<?php

function portal_games_delete () {
  
    menu_tree_set_path('microgame_portal', 'portal/admin');
    $arg = arg();
    $gid = $arg[4];
    $name = db_select('microgame_games', 'm')->fields('m', array('name'))->condition('m.gid', $gid, '=')->execute()->fetchField();
    if ($name) {
      // delete file versions
      $game = new Game($gid);
      foreach ($game->version_list() as $version){
        $file = file_load($version->fid);
        file_usage_delete($file,'portal_manager');
        file_delete($file);
      }
      db_delete('microgame_game_versions')->condition('gid', $gid)->execute();
      db_delete('microgame_games')->condition('gid', $gid)->execute();
      db_delete('microgame_subdomain_games')->condition('gid', $gid)->execute();
    }
    
    
    drupal_set_message(t('Game "@game" deleted',array('@game'=>$name)));
    drupal_goto('portal/admin/games');
}
