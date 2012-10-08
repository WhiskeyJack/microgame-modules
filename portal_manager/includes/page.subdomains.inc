<?php

/**
 * @file
 * Content for page with portal subdomain list
 */
function portal_subdomain_page() {

  menu_tree_set_path('microgame_portal', 'portal/admin');

  // get all themes
  $all_themes = list_themes();
  $theme_list = array();
  foreach ($all_themes as $theme_name => $theme) {
    $theme_list[$theme_name] = $theme->info['name'];
  }

  $header = array(
      array('data' => t('ID'), 'field' => 'sdid', 'sort' => 'desc', 'class' => array('mg_center')),
      array('data' => t('Name'), 'field' => 'name', 'class' => array('mg_center')),
      array('data' => t('Group name'), 'field' => 'group_name', 'class' => array('mg_center')),
      array('data' => t('Theme'), 'field' => 'theme_name', 'class' => array('mg_center')),
      array('data' => t('Games'), 'field' => 'game_count', 'class' => array('mg_center')),
      array('data' => t('Created by'), 'field' => 'created_by_name', 'class' => array('mg_center')),
      array('data' => t('Updated by'), 'field' => 'updated_by_name', 'class' => array('mg_center')),
      array('data' => t('Created on'), 'field' => 'created_on', 'class' => array('mg_center')),
      array('data' => t('Updated on'), 'field' => 'updated_on', 'class' => array('mg_center')),
  );

  $query = db_select('microgame_subdomains', 'm');
  $query->join('users', 'u', 'm.created_by_uid = u.uid');
  $query->join('users', 'v', 'm.updated_by_uid = v.uid');
  $query->leftJoin('microgame_subdomain_games', 'sg', 'm.sdid = sg.sdid');
  $query->fields('m');
  $query->addField('u', 'name', 'created_by_name');
  $query->addField('v', 'name', 'updated_by_name');
  $query->addExpression('COUNT (sg.gid)', 'game_count');
  $query->groupBy('m.sdid');
  $details = $query->extend('PagerDefault')->limit(10)->extend('TableSort')->orderByHeader($header)->execute()->fetchAssoc();

//  /$query = db_select('microgame_subdomains', 'm')->fields('m')->extend('PagerDefault')->limit(10)->extend('TableSort')->orderByHeader($header);
  $result = $query->execute()->fetchallAssoc('sdid');
$string =$query->__toString();
  $query = db_select('microgame_games', 'm');
  $query->fields('m', array('gid'));
  $query->addExpression('COUNT (m.gid)', 'game_count');
  $gamecount = $query->execute()->fetchField(1);

  $rows = array();
  foreach ($result as $row) {
    if (!isset($row->created_on)) {
      continue;
    }
    $fields = array();
    foreach ($header as $col) {
      if ($col['field'] == 'created_on' || $col['field'] == 'updated_on') {
        $fields[$col['field']] = array('data' => format_date($row->$col['field'], 'short'), 'class' => array('mg_center'));
      } else if ($col['field'] == 'theme_name') {
        $fields[$col['field']] = $theme_list[$row->$col['field']];
      } else {
        $fields[$col['field']] = array('data' => $row->$col['field'], 'class' => array('mg_center'));
      }
    }
    if ($gamecount > 0) {
      $gamelink = l(t('games'), "portal/admin/subdomains/{$row->name}/games/");
    } else {
      $gamelink ='';
    }
    $fields['edit'] = l(t('edit'), 'portal/admin/subdomains/edit/' . $row->name);
    $fields['games'] = $gamelink;
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

  $link = l(t('Create a new subdomain'), 'portal/admin/subdomains/create');

  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => "<p>{$link}</p>",
      ),
      'second_para' => array(
          '#theme' => 'table',
          '#header' => $header,
          '#attributes' => array('class' => array('mg_table', 'subdomain_table')),
          '#empty' => t('No subdomains have been configured'),
          '#rows' => $rows,
      ),
  );
  return $output;
}

?>
