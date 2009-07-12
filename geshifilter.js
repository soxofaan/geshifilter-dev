var geshifilter_collapse_height = "100px";

$(document).ready(function() {
  $('div.geshifilter').each(function() {
    // Add more/less link.
    $(this).append('<div class="geshifilter-tools"><a href="#" class="geshifilter-tools-toggle">Read more</a></div>');

    var code_container = $(this).children(':first');
    // Store the original heights of the geshifilter divs.
    code_container.attr('original_height', code_container.height());
    // Collapse by default.
    code_container.addClass('geshifilter-collapsed').css("height", geshifilter_collapse_height);
  });

  // Add handler to read more/collapse link.
  $("a.geshifilter-tools-toggle").click(function() {
    // Get code container.
    var code_container = $(this).parent().prev();

    if (code_container.hasClass('geshifilter-collapsed')) {
      // Show more (use original height).
      var original_height = code_container.attr('original_height') + 'px';
      code_container.removeClass('geshifilter-collapsed').animate({height: original_height}, 'slow');
      $(this).html('Show less');
    }
    else {
      // Collapse.
      code_container.addClass('geshifilter-collapsed').animate({height: geshifilter_collapse_height}, 'slow');
      $(this).html('Read more');
    }
    // Return false so that the link is no followed.
    return false;
  });

});
