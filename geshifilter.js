
$(document).ready(function() {
  // Add more/less link.
  $('div.geshifilter').append('<div class="geshifiltertools"><a href="#" class="geshifilter-tools-toggle">show more/less</a></div>');
  // Hide by default.
  $('div.geshifiltertools').prev().hide();
  // Add toggle effect to more/less link.
  $('a.geshifilter-tools-toggle').click(function(){
    $(this).parent().prev().toggle('slow');
    // Return false so that the link is no followed.
    return false;
  });
});
