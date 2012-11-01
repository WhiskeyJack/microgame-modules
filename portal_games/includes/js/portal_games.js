(function ($) {
  $(document).ready(function(){
    var gameurl = Drupal.settings.param.gameurl;
    var videourl = Drupal.settings.param.videourl;
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
    $(".game_video_btn").click(function() {
      $.fancybox.open({
        href : videourl,
        type : 'swf',
        scrolling: 'no',
        preload: true,
        closeBtn: true,
        padding: 0,
        helpers : {
          overlay : {
            media: {}
          }
        }
      });
    });
  });
})(jQuery);