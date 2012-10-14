(function ($) {
  $(document).ready(function(){
    var gameurl = Drupal.settings.param.gameurl;    
    var closebtn = Drupal.settings.param.closebtn == 1 ? true : false;
    $(".game_play_btn").click(function() {
      $.fancybox.open({
        href : gameurl,
        type : 'iframe',
        width: 1024,
        height: 768,
        scrolling: 'no',
        closeBtn: closebtn,
        padding: 0,
        helpers : {
          overlay : {
            closeClick: false
          }
        }
      });
    });
  });
})(jQuery);