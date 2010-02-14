// $Id: geshifilter.js,v 1.1 2010/02/14 23:31:09 soxofaan Exp $

// Define a Drupal behavior for with jQuery magic for
// partial collapsing/showing GeSHi filter code blocks.
Drupal.behaviors.geshiFilterCodeCollapsing = function (context) {
  // The height to collapse to.
  var geshifilter_collapse_height = 100;
  var geshifilter_collapse_height_threshold = 150;

  // Add the expand/collapse toggle to geshifilter code blocks.
  $('div.geshifilter.collapsible').each(function() {
    // Get code container and its height.
    var code_container = $(this);
    var code_container_original_height = code_container.height();

    // Only do the collapsing where it makes sense.
    if (code_container_original_height > geshifilter_collapse_height_threshold) {
      // Add more/less link.
      $(this).after('<div class="geshifilter-tools"><a href="#" class="geshifilter-tools-collapsetoggle">' + Drupal.t('Show all code') + '</a></div>');
      // Store the original heights of the geshifilter divs.
      code_container.attr('original_height', code_container_original_height);
      // Collapse by default.
      code_container.addClass('geshifilter-collapsed').css("height", geshifilter_collapse_height);
    }
  });

  // Add handler to read more/collapse link.
  $("a.geshifilter-tools-collapsetoggle").click(function() {
    // Get code container.
    var code_container = $(this).parent().prev();

    if (code_container.hasClass('geshifilter-collapsed')) {
      // Show more (use original height).
      var original_height = code_container.attr('original_height') + 'px';
      code_container.removeClass('geshifilter-collapsed').animate({height: original_height});
      $(this).text(Drupal.t('Show less code'));
    }
    else {
      // Collapse.
      code_container.addClass('geshifilter-collapsed').animate({height: geshifilter_collapse_height});
      $(this).text(Drupal.t('Show all code'));
    }
    // Return false so that the link is no followed.
    return false;
  });
}
