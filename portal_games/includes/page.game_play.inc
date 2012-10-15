<?php
/**
 * @file
 * Page that will load the actual game
 */
function portal_game_play($game) {
  
  $mpath = drupal_get_path('module', 'portal_games');
  drupal_add_js($mpath . '/includes/js/unityObject.js', array('weight' => 1));
  
//  $unity_url = $mpath . '/includes/js/unityObject.js';
  
  $file = file_load($game->fid());
  $url = file_create_url($file->uri);
  
  $game->config(1);
  
  $install_text = t('Please install the Unity webplayer to play this game.');

    $js = '<script>var params = {
			backgroundcolor: "eaeaea",
			bordercolor: "000000",
			textcolor: "FFFFFF",
			logoimage: "http://office.inthere.nl/microgame_small.png",
			progressbarimage: "http://office.inthere.nl/ProgressBarPurple4.png",
			progressframeimage: "http://office.inthere.nl/ProgressBarFrame.png"
		};
              var uniObj = new UnityObject("' . $url . '", "unity", "1024", "768", "2");
              uniObj.setAttribute("altHTML", "' . $install_text . '");
		uniObj.addParam("logoimage", "../images/microgame_small.png");
		uniObj.addParam("progressbarimage", "../images/ProgressBarPurple.png");
		uniObj.addParam("progressframeimage", "../images/ProgressBarFrame.png");
              uniObj.write();</script> ';
  
  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => $js,
      ),
  );
  return $output;
}