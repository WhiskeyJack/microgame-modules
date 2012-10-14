<?php

/**
 * @file
 * Contains the form adding games to a subdomain and enabling them.
 */
function subdomain_game_edit_form($form, &$form_state, $subdomain = '') {
  menu_tree_set_path('microgame_portal', 'portal/admin');

  
  $arg = arg();
  $name = $arg[count($arg) - 2];
  $sdid = subdomain()->name_exists($name);

  drupal_set_title(t('Configured games for @subdomain', array('@subdomain' =>$name)));
  
  $query = db_select('microgame_games', 'm');
  $query->leftJoin('users', 'u', 'm.created_by_uid = u.uid');
  $query->leftJoin('users', 'v', 'm.updated_by_uid = v.uid');
  $query->leftJoin('microgame_game_versions', 'gv', 'm.gid = gv.gid');
  $query->fields('m');
  $query->addField('u', 'name', 'created_by_name');
  $query->addField('v', 'name', 'updated_by_name');
  $query->addExpression('COUNT (gv.version_id)', 'version_count');
  $query->groupBy('m.gid');
  $all_games = $query->orderBy('name', 'ASC')->execute()->fetchallAssoc('gid');

  $currentgames = db_select('microgame_subdomain_games', 'm')
          ->fields('m')
          ->condition('sdid', $sdid)
          ->orderBy('weight', 'ASC')
          ->execute()
          ->fetchallAssoc('gid');

  $gamelist = $currentgames;

  foreach ($currentgames as $gid => $game) {
    $gamelist[$gid] = $all_games[$gid];
    $gamelist[$gid]->weight = $game->weight;
    $gamelist[$gid]->enabled = $game->enabled;
    $gamelist[$gid]->closebtn = $game->closebtn;
    unset($all_games[$gid]);
  }

  $item = new stdClass();
  $item->name = t('Not available in this domain');
  $item->description = "";
  $item->gid = 0;
  $gamelist[0] = $item;

  $gamelist += $all_games;

  $form['game_items']['#tree'] = TRUE;
  $form['game_items']['sdid'] = array('#type' => 'value', '#value' => $sdid);

  foreach ($gamelist as $item) {
    if (!isset($item->weight)) {
      $item->weight = 10;
    }
    if (!isset($item->enabled)) {
      $item->enabled = 1;
    }
    if (!isset($item->closebtn)) {
      $item->closebtn = 1;
    }

    
    $access = TRUE;
    if ($item->gid == 0) {
      $access = FALSE;
    }

    $form['game_items'][$item->gid] = array(
        'name' => array(
            '#markup' => check_plain($item->name) . '&nbsp;&nbsp;',
        ),
        'enabled' => array(
            '#type' => 'checkbox',
            '#default_value' => $item->enabled,
            '#access' => $access,
        ),
        'closebtn' => array(
            '#type' => 'checkbox',
            '#default_value' => $item->closebtn,
            '#access' => $access,
        ),
        'weight' => array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#default_value' => $item->weight,
            '#delta' => 50,
            '#title-display' => 'invisible',
        ),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));
  return $form;
}

function theme_subdomain_game_edit_form($variables) {
  $form = $variables['form'];

  $rows = array();

  // Iterate over each element in our $form['game_items'] array
  foreach (element_children($form['game_items']) as $id) {

    // Before we add our 'weight' column to the row, we need to give the
    // element a custom class so that it can be identified in the
    // drupal_add_tabledrag call.
    //
    $form['game_items'][$id]['weight']['#attributes']['class'] = array('item-weight');

    // We are now ready to add each element of our $form data to the $rows
    // array, so that they end up as individual table cells when rendered
    // in the final table.  We run each element through the drupal_render()
    // function to generate the final html markup for that element.
    $draggable = $id != 0 ? array('draggable') : array();
    $rows[] = array(
        'data' => array(
            drupal_render($form['game_items'][$id]['name']),
            drupal_render($form['game_items'][$id]['enabled']),
            drupal_render($form['game_items'][$id]['closebtn']),
            drupal_render($form['game_items'][$id]['weight']),
        ),
        'class' => $draggable,
    );
  }

  // We now define the table header values.  Ensure that the 'header' count
  // matches the final column count for your table.
  $header = array(t('Name'), t('Enabled'),t('Close Button'), t('Weight'));

  // We also need to pass the drupal_add_tabledrag() function an id which will
  // be used to identify the <table> element containing our tabledrag form.
  // Because an element's 'id' should be unique on a page, make sure the value
  // you select is NOT the same as the form ID used in your form declaration.
  $table_id = 'subdomain-game-table';

  // We can render our tabledrag table for output.
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));

  // And then render any remaining form elements (such as our submit button)
  $output .= drupal_render_children($form);

  // For a basic sortable table, we need to pass it:
  //   - the $table_id of our <table> element,
  //   - the $action to be performed on our form items ('order'),
  //   - a string describing where $action should be applied ('siblings'),
  //   - and the class of the element containing our 'weight' element.
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'item-weight');

  return $output;
}

function subdomain_game_edit_form_submit($form, &$form_state) {
  $sdid = $form_state['values']['game_items']['sdid'];
  unset($form_state['values']['game_items']['sdid']);
  $name = db_select('microgame_subdomains', 'm')->fields('m', array('name'))->condition('m.sdid', $sdid, '=')->execute()->fetchField();

  if ($sdid != 0) {
    $nullweight = $form_state['values']['game_items'][0]['weight'];
    db_delete('microgame_subdomain_games')->condition('sdid', $sdid)->execute();
    foreach ($form_state['values']['game_items'] as $gid => $val) {
      if ($val['weight'] < $nullweight) {
        db_insert('microgame_subdomain_games')
                ->fields(array(
                    'sdid' => $sdid,
                    'gid' => $gid,
                    'weight' => $val['weight'],
                    'enabled' => $val['enabled'],
                    'closebtn' => $val['closebtn'],
                ))
                ->execute();
      }
    }
  }

  drupal_set_message(t('Game selection for subdomain "@name" has been updated', array('@name' => $name)));
  drupal_goto('portal/admin/subdomains');
}