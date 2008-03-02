<?php
// $Id$

require_once('geshifilter.inc');

/**
 * geshifilter_filter callback for preparing input text.
 */
function _geshifilter_prepare($format, $text) {
  // get the available tags
  list($generic_code_tags, $language_tags, $tag_to_lang) = _geshifilter_get_tags($format);
  $tags = array_merge($generic_code_tags, $language_tags);
  // escape special (regular expression) characters in tags (for tags like 'c++' and 'c#')
  $tags = preg_replace('#(\\+|\\#)#', '\\\\$1', $tags);
  $tags_string = implode('|', $tags);
  // Pattern for matching "<code>...</code>" like stuff
  // Also matches "<code>...$"  where "$" refers to end of string, not end of
  // line (because PCRE_MULTILINE (modifier 'm') is not enabled), so matching
  // still works when teaser view trims inside the source code.
  switch (_geshifilter_brackets($format)) {
    case GESHIFILTER_BRACKETS_ANGLE:
      $pattern = '#(<)('. $tags_string .')((\s+[^>]*)*)(>)(.*?)(</\2\s*>|$)#s';
      break;
    case GESHIFILTER_BRACKETS_SQUARE:
      $pattern = '#(\[)('. $tags_string .')((\s+[^\]]*)*)(\])(.*?)(\[/\2\s*\]|$)#s';
      break;
    case GESHIFILTER_BRACKETS_BOTH:
      $pattern = '#([<\[])('. $tags_string .')((\s+[^>\]]*)*)([>\]])(.*?)(\1/\2\s*\5|$)#s';
      break;
  }
  // replace the code container tag brackets
  // and prepare the container content (newline and angle bracket protection)
  $text = preg_replace_callback($pattern, '_geshifilter_prepare_callback', $text);
  if (_geshifilter_php_delimeters($format)) {
    // prepare < ?php ... ? > blocks
    $text = preg_replace_callback('#[\[<](\?php|\?PHP|%)(.+?)(\?|%)[\]>]#s', '_geshifilter_prepare_php_callback', $text);
  }
  return $text;
}

/**
 * _geshifilter_prepare callback for preparing input text.
 * Replaces the code tags brackets with geshifilter specific ones to prevent
 * possible messing up by other filters, e.g.
 *   '[python]foo[/python]' to '[geshifilter-python]foo[/geshifilter-python]'.
 * Replaces newlines with "&#10;" to prevent issues with the line break filter
 * Escapes the tricky characters like angle brackets with check_plain() to
 * prevent messing up by other filters like the HTML filter.
 */
function _geshifilter_prepare_callback($match) {
  // get the default highlighting mode
  $lang = variable_get('geshifilter_default_highlighting', GESHIFILTER_DEFAULT_PLAINTEXT);
  if ($lang == GESHIFILTER_DEFAULT_DONOTHING) {
    // If the default highlighting mode is GESHIFILTER_DEFAULT_DONOTHING
    // and there is no language attribute, we should not do any escaping in this
    // prepare phase, so that other filters can do their thing.
    $language_attributes = _geshifilter_whitespace_explode(GESHIFILTER_ATTRIBUTES_LANGUAGE);
    $enabled_languages = _geshifilter_get_enabled_languages();
    $attributes_preg_string = implode('|', $language_attributes);
    // parse attributes
    preg_match_all('#('. $attributes_preg_string .')="?([^\s"]*)"?#', $match[2], $attribute_matches);
    foreach ($attribute_matches[1] as $a_key => $att_name) {
      $att_value = $attribute_matches[2][$a_key];
      // check for the language attributes
      if (in_array($att_name, $language_attributes) && array_key_exists($att_value, $enabled_languages)) {
        $lang = $att_value;
      }
    }
    if ($lang == GESHIFILTER_DEFAULT_DONOTHING) {
      // no language set by attributes: prevent escaping and return original string
      return $match[0];
    }
  }
  // return escaped code block
  return '[geshifilter-'. $match[2] . $match[3] .']'
    . str_replace(array("\r", "\n"), array('', '&#10;'), check_plain($match[6]))
    .'[/geshifilter-'. $match[2] .']';
}

/**
 * _geshifilter_prepare callback for < ?php ... ? > blocks
 */
function _geshifilter_prepare_php_callback($match) {
  return '[geshifilter-questionmarkphp]'
    . str_replace(array("\r", "\n"), array('', '&#10;'), check_plain($match[2]))
    .'[/geshifilter-questionmarkphp]';
}

/**
 * geshifilter_filter callback for processing input text.
 */
function _geshifilter_process($format, $text) {
  // load GeSHi library (if not already)
  $geshi_library = _geshifilter_check_geshi_library();
  if (!$geshi_library['success']) {
    drupal_set_message($geshi_library['message'], 'error');
    return $text;
  }
  // get the available tags
  list($generic_code_tags, $language_tags, $tag_to_lang) = _geshifilter_get_tags($format);
  if (_geshifilter_php_delimeters($format)) {
    $language_tags[] = 'questionmarkphp';
    $tag_to_lang['questionmarkphp'] = 'php';
  }
  $tags = array_merge($generic_code_tags, $language_tags);
  // escape special (regular expression) characters in tags (for tags like 'c++' and 'c#')
  $tags = preg_replace('#(\\+|\\#)#', '\\\\$1', $tags);
  $tags_string = implode('|', $tags);
  // Pattern for matching the prepared "<code>...</code>" stuff
  $pattern = '#\\[geshifilter-('. $tags_string .')([^\\]]*)\\](.*?)(\\[/geshifilter-\1\\])#s';
  $text = preg_replace_callback($pattern, create_function('$match', "return _geshifilter_replace_callback(\$match, $format);"), $text);
  return $text;
}

/**
 * preg_replace_callback callback
 */
function _geshifilter_replace_callback($match, $format) {
  // $match[0]: complete matched string
  // $match[1]: tag name
  // $match[2]: tag attributes
  // $match[3]: tag content
  $complete_match = $match[0];
  $tag_name = $match[1];
  $tag_attributes = $match[2];
  $source_code = $match[3];

  // Undo linebreak and escaping from preparation phase
  $source_code = decode_entities($source_code);

  // get the possible tags and languages
  list($generic_code_tags, $language_tags, $tag_to_lang) = _geshifilter_get_tags($format);
  if (variable_get('geshifilter_enable_php_delimiters', FALSE)) {
    $language_tags[] = 'questionmarkphp';
    $tag_to_lang['questionmarkphp'] = 'php';
  }

  $enabled_languages = _geshifilter_get_enabled_languages();

  $language_attributes = _geshifilter_whitespace_explode(GESHIFILTER_ATTRIBUTES_LANGUAGE);
  $attributes_preg_string = implode('|', array_merge(
    $language_attributes,
    array(GESHIFILTER_ATTRIBUTE_LINE_NUMBERING, GESHIFILTER_ATTRIBUTE_LINE_NUMBERING_START, GESHIFILTER_ATTRIBUTE_FANCY_N)
  ));

  // default settings
  $lang = variable_get('geshifilter_default_highlighting', GESHIFILTER_DEFAULT_PLAINTEXT);
  $line_numbering = variable_get('geshifilter_default_line_numbering', GESHIFILTER_LINE_NUMBERS_DEFAULT_NONE);
  $linenumbers_start = 1;

  // determine language based on tag name if possible
  if (isset($tag_to_lang[$tag_name])) {
    $lang = $tag_to_lang[$tag_name];
  }

  // parse attributes (for language and line numbers)
  preg_match_all('#('. $attributes_preg_string .')="?([^\s"]*)"?#', $tag_attributes, $attribute_matches);
  // $attribute_matches[0][xx] .... fully matched string type="language"
  // $attribute_matches[1][xx] .... param name
  // $attribute_matches[2][xx] .... param value
  foreach ($attribute_matches[1] as $a_key => $att_name) {
    $att_value = $attribute_matches[2][$a_key];
    // check for the language attributes
    if (in_array($att_name, $language_attributes)) {
      // map attribute value to geshi language code if possible
      if (in_array($att_value, $language_tags)) {
        $att_value = $tag_to_lang[$att_value];
      }
      // set language if extracted language is an enabled language
      if (array_key_exists($att_value, $enabled_languages)) {
        $lang = $att_value;
      }
    }
    // check for line numbering related attributes
    // $line_numbering defines the line numbering mode:
    // 0: no line numbering
    // 1: normal line numbering
    // n>= 2: fancy line numbering every nth line
    elseif ($att_name == GESHIFILTER_ATTRIBUTE_LINE_NUMBERING) {
      switch (strtolower($att_value)) {
        case "off":
          $line_numbering = 0;
          break;
        case "normal":
          $line_numbering = 1;
          break;
        case "fancy":
          $line_numbering = 5;
          break;
      }
    }
    elseif ($att_name == GESHIFILTER_ATTRIBUTE_FANCY_N) {
      $att_value = (int)($att_value);
      if ($att_value >= 2) {
        $line_numbering = $att_value;
      }
    }
    elseif ($att_name == GESHIFILTER_ATTRIBUTE_LINE_NUMBERING_START) {
      if ($line_numbering < 1) {
        $line_numbering = 1;
      }
      $linenumbers_start = (int)($att_value);
    }
  }

  if ($lang == GESHIFILTER_DEFAULT_DONOTHING) {
    // do nothing, and return the original
    return $complete_match;
  }

  if ($lang == GESHIFILTER_DEFAULT_PLAINTEXT) {
    // use plain text 'highlighting'
    $lang = 'text';
  }
  $inline_mode = (strpos($source_code, "\n") === FALSE);
  // process and return
  return geshifilter_process($source_code, $lang, $line_numbering, $linenumbers_start, $inline_mode);
}

/**
 * Helper function for overriding some GeSHi defaults
 */
function _geshifilter_override_geshi_defaults(&$geshi, $langcode) {
  // override the some default GeSHi styles (e.g. GeSHi uses Courier by default, which is ugly)
  $geshi->set_line_style('font-family: monospace; font-weight: normal;', 'font-family: monospace; font-weight: bold; font-style: italic;');
  $geshi->set_code_style('font-family: monospace; font-weight: normal; font-style: normal');
  // overall class needed for CSS
  $geshi->set_overall_class('geshifilter-'. $langcode);
  // set keyword linking
  $geshi->enable_keyword_links(variable_get('geshifilter_enable_keyword_urls', TRUE));
}

/**
 * General geshifilter processing function
 */
function geshifilter_process($source_code, $lang, $line_numbering=0, $linenumbers_start=1, $inline_mode=FALSE) {
  // process
  if ($lang == 'php' && variable_get('geshifilter_use_highlight_string_for_php', FALSE)) {
    return geshifilter_highlight_string_process($source_code, $inline_mode);
  }
  else {
    // process with GeSHi
    return geshifilter_geshi_process($source_code, $lang, $line_numbering, $linenumbers_start, $inline_mode);
  }
}

/**
 * geshifilter wrapper for GeSHi processing.
 */
function geshifilter_geshi_process($source_code, $lang, $line_numbering=0, $linenumbers_start=1, $inline_mode=FALSE) {
  // load GeSHi library (if not already)
  $geshi_library = _geshifilter_check_geshi_library();
  if (!$geshi_library['loaded']) {
    drupal_set_message($geshi_library['message'], 'error');
    return $source_code;
  }
  // remove leading/trailing newlines
  $source_code = trim($source_code, "\n\r");
  // create GeSHi object
  $geshi = _geshifilter_geshi_factory($source_code, $lang);

  // CSS mode
  if (_geshifilter_css_mode() == GESHIFILTER_CSS_CLASSES) {
    $geshi->enable_classes(TRUE);
  }
  _geshifilter_override_geshi_defaults($geshi, $lang);
  // some more GeSHi settings and parsing
  if ($inline_mode) {
    // inline source code mode
    $geshi->set_header_type(GESHI_HEADER_NONE);
    $source_code = '<span class="geshifilter"><code class="'. $geshi->overall_class .'">'. $geshi->parse_code() .'</code></span>';
  }
  else {
    // block source code mode
    $geshi->set_header_type((int)variable_get('geshifilter_code_container', GESHI_HEADER_PRE));
    if ($line_numbering == 1) {
      $geshi->enable_line_numbers(GESHI_NORMAL_LINE_NUMBERS);
      $geshi->start_line_numbers_at($linenumbers_start);
    }
    elseif ($line_numbering >= 2) {
      $geshi->enable_line_numbers(GESHI_FANCY_LINE_NUMBERS, $line_numbering);
      $geshi->start_line_numbers_at($linenumbers_start);
    }
    $source_code = '<div class="geshifilter">'. $geshi->parse_code() .'</div>';
  }
  return $source_code;
}

/**
 * geshifilter wrapper for highlight_string() processing of PHP
 */
function geshifilter_highlight_string_process($source_code, $inline_mode) {
  // Make sure that the source code starts with < ?php and ends with ? >
  $text = trim($source_code);
  if (substr($text, 0, 5) != '<?php') {
    $source_code = '<?php'. $source_code;
  }
  if (substr($text, -2) != '?>') {
    $source_code = $source_code .'?>';
  }
  // Use the right container
  $container = $inline_mode ? 'span' : 'div';
  // Process with highlight_string()
  $text = '<'. $container .' class="codeblock geshifilter">'. highlight_string($source_code, TRUE) .'</'. $container .'>';
  // Remove newlines (added by highlight_string()) to avoid issues with the linebreak filter
  $text = str_replace("\n", '', $text);
  return $text;
}
