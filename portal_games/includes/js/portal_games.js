(function ($) {
  $(document).ready(function(){
    var gameurl = Drupal.settings.param.gameurl;
    $(".game_play_btn").click(function() {
      $.fancybox.open({
        href : gameurl,
        type : 'iframe',
        width: 1024,
        height: 768,
        scrolling: 'no',
        closeBtn: false,
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