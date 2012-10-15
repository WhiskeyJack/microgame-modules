<?php

function portal_games_page() {


  menu_tree_set_path('microgame_portal', 'portal/admin');

  $breadcrumbs = array(
      l(t('Home'), '<front>'),
      l(t('Configuration'), 'portal/admin'),
  );
  drupal_set_breadcrumb($breadcrumbs);
  
  // get all themes
  $all_themes = list_themes();
  $theme_list = array();
  foreach ($all_themes as $theme_name => $theme) {
    $theme_list[$theme_name] = $theme->info['name'];
  }

  $header = array(
      array('data' => t('ID'), 'field' => 'gid', 'sort' => 'desc'),
      array('data' => t('Name'), 'field' => 'name', 'class' => array('mg_center')),
      array('data' => t('Active version'), 'field' => 'active_version'),
      array('data' => t('Versions'), 'field' => 'version_count'),
      array('data' => t('Subdomains'), 'field' => 'subdomain_count'),
      array('data' => t('Created by'), 'field' => 'created_by_name', 'class' => array('mg_center')),
      array('data' => t('Updated by'), 'field' => 'updated_by_name', 'class' => array('mg_center')),
      array('data' => t('Created on'), 'field' => 'created_on', 'class' => array('mg_center')),
      array('data' => t('Updated on'), 'field' => 'updated_on', 'class' => array('mg_center')),
  );

  $query = db_select('microgame_games', 'm');
  $query->leftJoin('users', 'u', 'm.created_by_uid = u.uid');
  $query->leftJoin('users', 'v', 'm.updated_by_uid = v.uid');
  $query->leftJoin('microgame_game_versions', 'gv', 'm.gid = gv.gid');
  $query->leftJoin('microgame_subdomain_games', 'sg', 'm.gid = sg.gid');
  $query->fields('m');
  $query->addField('u', 'name', 'created_by_name');
  $query->addField('v', 'name', 'updated_by_name');
  $query->addExpression('COUNT (gv.version_id)', 'version_count');
  $query->addExpression('COUNT (sg.sdid)', 'subdomain_count');
  $query->groupBy('m.gid');
  $result = $query->extend('PagerDefault')->limit(10)->extend('TableSort')->orderByHeader($header)->execute()->fetchallAssoc('gid');

  $rows = array();
  foreach ($result as $row) {
    $fields = array();
    foreach ($header as $col) {
      if ($col['field'] == 'created_on' || $col['field'] == 'updated_on') {
        $fields[$col['field']] = array('data' => format_date($row->$col['field'], 'short'), 'class' => array('mg_center'));
      } elseif ($col['field'] == 'gid' || $col['field'] == 'active_version' || $col['field'] == 'version_count' || $col['field'] == 'subdomain_count' ) {
        $fields[$col['field']] = array('data' => $row->$col['field'], 'class' => array('mg_center'));
      } else {
        $fields[$col['field']] = $row->$col['field'];
      }
    }
    $fields['edit'] = l(t('edit'), 'portal/admin/games/edit/' . $row->gid);
    if ($row->subdomain_count > 0) {
      $text = t('This game is still available in @subdomains subdomain(s)!\nAre you sure you want to delete this game and all versions?', 
              array('@subdomains' => $row->subdomain_count));
    } else {
      $text = t('Are you sure you want to delete this game and all versions?');
    }
    $command =  'return confirm("' . $text . '"); ';
    $fields['delete'] = l(t('delete'), 'portal/admin/games/delete/' . $row->gid,  array('attributes' => array('onclick' =>$command)));
    $rows[] = $fields;
  }

  /*
    $table['header'] = $header;
    $table['attributes'] = array('class' => array('mg_table', 'subdomain_table'));
    $table['caption'] = '';
    $table['colgroups'] = array();
    $table['sticky'] = "";
    $table['empty'] = t('No subdomains have been configured');
    $table['rows'] = $rows;
    return theme_table($table);
   */

  $link = l(t('Create a game'), 'portal/admin/games/upload');

  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => "<p>{$link}</p>",
      ),
      'second_para' => array(
          '#theme' => 'table',
          '#header' => $header,
          '#attributes' => array('class' => array('mg_table', 'gamelist_table')),
          '#empty' => t('No games have been configured'),
          '#rows' => $rows,
      ),
  );
  return $output;
}
