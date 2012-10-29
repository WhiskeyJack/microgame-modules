<?php
/**
 * @file
 * Page that will load the actual game
 */
function portal_game_play($game) {
  
  $sdid = subdomain()->id();
  $mpath = drupal_get_path('module', 'portal_games');
  drupal_add_js($mpath . '/includes/js/unityObject.js', array('weight' => 1));
  
//  $unity_url = $mpath . '/includes/js/unityObject.js';
  
  $file = file_load($game->fid());
  $url = file_create_url($file->uri);
  
  $game->start_game($sdid);
  
  $install_text = t('Please install the Unity webplayer to play this game.');

  $imagedir= path_to_theme() . '/images/unityloader/';
  $logo = url($imagedir . 'microgame_small.png', array('absolute' => TRUE));
  $bar = url($imagedir . 'ProgressBarPurple.png', array('absolute' => TRUE));
  $frame = url($imagedir . 'ProgressBarFrame.png', array('absolute' => TRUE));
  
  //mg_debug(url($frame, array('absolute' => TRUE)));
  
    $js = '<script>var params = {
			backgroundcolor: "eaeaea",
			bordercolor: "000000",
			textcolor: "FFFFFF",
			logoimage: "' . $logo . '",
			progressbarimage: "' . $bar . '",
			progressframeimage: "' . $frame . '"
		};
              var uniObj = new UnityObject("' . $url . '", "unity", "1024", "768", "2");
              uniObj.setAttribute("altHTML", "' . $install_text . '");
		uniObj.addParam("logoimage", "' . $logo . '");
		uniObj.addParam("progressbarimage", "' . $bar . '");
		uniObj.addParam("progressframeimage", "' . $frame . '");
              uniObj.write();</script> ';
  
  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => $js,
      ),
  );
  
  return $output;
}