$(document).ready(function() {
  // Send AJAX query regarding screen resolution:
  if ($) {
    $.ajax({
      async: true,
      cache: false,
      data: {x: window.screen.width, y: window.screen.height},
      dataType: 'json',
      type: 'GET',
      url: '/resolution.php',
      success: function(data, tstatus, req) {
      },
      error: function(req, tstatus, err) {
      }
    });
  }
  if (window.location.href.substring(0,5) == 'https') {
    if (window.location.href.substring(0,28) != "https://www.aarongifford.com") {
      window.location = 'https://www.aarongifford.com/';
    }
  } else if (window.location.href.substring(0,4) == 'http') {
    if (window.location.href.substring(0,27) != "http://www.aarongifford.com") {
      window.location = 'http://www.aarongifford.com/';
    }
  }
});
