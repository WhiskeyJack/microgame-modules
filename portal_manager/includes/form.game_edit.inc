<?php

/**
 * @file
 * Contains the form for editing and uploading a game.
 */
function game_edit_form($form, &$form_state, $gid = 0) {
  menu_tree_set_path('microgame_portal', 'portal/admin');

  $breadcrumbs = array(
      l(t('Home'), '<front>'),
      l(t('Configuration'), 'portal/admin'),
      l(t('Games'), 'portal/admin/games'),
  );
  drupal_set_breadcrumb($breadcrumbs);
  
  $defaults = array('name' => '', 'description' => '', 'gid' => 0, 'youtube_id' => '');
  
  $arg = arg();
  if ($arg[count($arg)-2] == 'edit' ) {
    // upload a new version
    $gid = end($arg);
    $game = new Game($gid);
    if ($game->gid() != 0) {
      $defaults = array(
          'name' => $game->name(), 
          'description' => $game->description(), 
          'gid' => $game->gid(),
          'youtube_id' => $game->youtube_id()
              );
      $form['lastedit'] = array(
          '#type' => 'item',
          '#markup' => t('This game was last edited on @date by @user.', array('@date' => format_date($game->updated_on()), '@user' => $game->updated_by_name()))
      );
    } else {
      drupal_set_message('Could not find a game for specified id', 'error');
      drupal_goto('portal/admin/games');
    }
  }
  
  $form['gid'] = array('#type' => 'value', '#value' => $defaults['gid']);
  
  $form['game_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Game name'),
      '#description' => t('The name of this game'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
      '#default_value' => $defaults['name'],
      '#element_validate' => array('mg_is_valid_game_name'),
  );

  $form['game_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Game description'),
      '#description' => t('Short description of the game'),
      '#cols' => 30,
      '#rows' => 4,
      '#resizable' => FALSE,
      '#required' => TRUE,
      '#default_value' => $defaults['description'],
  );
  
  $form['youtube_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Youtube video ID'),
      '#description' => t('The 11 character youtube identifier (found after "v="  in the youtube url)'),
      '#size' => 12,
      '#maxlength' => 11,
      '#required' => FALSE,
      '#default_value' => $defaults['youtube_id'],
      '#element_validate' => array('mg_is_valid_youtube_id'),
  );
  
  if ($defaults['gid'] != 0 ) {
    $form['active_version'] = array(
      '#type' => 'select',
      '#title' => t('Active version'),
      '#description' => t('Select the active version for this game'),      
      '#options' => $game->version_select_list(),
        '#default_value' => $game->version()
  );
  }
  
  $form['fid'] = array(
      '#title' => t('Game file'),
      '#type' => 'file',
      '#size' => 50,
      '#required' => TRUE,
      '#description' => t('File must have a .unity3d extension'),
  );
    
  $form['make_active'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make this version active'),
      '#description' => t('If set this will become the active version for this game'),      
      '#default_value' => 1,
      '#disabled' => $defaults['gid'] != 0 ? FALSE : TRUE
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
  );

  return $form;
}

function game_edit_form_validate($form, &$form_state) {
  $file = file_save_upload('fid', array(
      'file_validate_extensions' => array(),
          ));

  if ($file) {
    // check extension
    $ext_check = file_validate_extensions($file, 'unity3d');
    if (empty($ext_check)) {
      $filename = mg_string_to_filename($form_state['values']['game_name']);
      // Get the image info to get the correct extension for the uploaded file.
      $info = pathinfo($file->filename);
      $movedfile = file_move($file, "private://games/{$filename}.{$info['extension']}", FILE_EXISTS_RENAME);
      if ($movedfile) {
        // Mark the file for permanent storage.
        $movedfile->status = FILE_STATUS_PERMANENT;
        file_save($movedfile);
        // now clear the error message for the file field (triggers as empty even if file was specified)
        mg_remove_form_element_errors('fid', $form, $form_state);
        form_set_value($form['fid'], $file->fid, $form_state);
      } else {
        form_set_error('fid', t('Failed to write the uploaded file to the site’s files folder.'));
      }
    } else {
      // wrong extension
      mg_remove_form_element_errors('fid', $form, $form_state);
      form_set_error('fid', current($ext_check));
      file_delete($file);
    }
  } else {
    if ($form_state['values']['gid'] ==0) {
      drupal_set_message('Something went wrong with the upload.', 'error');
    }
    else {
      // accept empty file upload for existing games 
      mg_remove_form_element_errors('fid', $form, $form_state);
      form_set_value($form['fid'], 0, $form_state);
    }
  }
}

function game_edit_form_submit($form, &$form_state) {
  
  
  if ($form_state['values']['gid'] == 0) {
    // new game
    $game = new Game();
    $gid = $game->new_game( $form_state['values']['game_name'], $form_state['values']['game_description'], $form_state['values']['fid'],  $form_state['values']['youtube_id'])->save()->gid();
    drupal_set_message(t('The game has been uploaded with game id @game_id.', array('@game_id' => $gid)));
  } else {
    // update existing game
    $game = new Game($form_state['values']['gid']);
    $make_active = $form_state['values']['make_active'] == 1 ? TRUE : FALSE;
    if (isset($form_state['values']['active_version'])) {
      $game->set_active($form_state['values']['active_version']);
    }
    $gid = $game->update_game( $form_state['values']['game_name'], $form_state['values']['game_description'], $form_state['values']['fid'],  $form_state['values']['youtube_id'], $make_active)
            ->save()->gid();
    if ($game->version_count() == 1) {
      drupal_set_message(t('The game with game id @game_id has been updated, there is  now 1 version available.',
            array('@game_id' => $gid)));
    } else {
      drupal_set_message(t('The game with game id @game_id has been updated, there are now @versioncount versions available.',
            array('@game_id' => $gid, '@versioncount' => $game->version_count())));
    }
  }
  drupal_goto('portal/admin/games');
  
}

function mg_is_valid_game_name($element, &$form_state) {

    if (!preg_match('=^[^/?*;:{}\\\\]+$=', $element['#value'])) {
    form_error($element, t('Invalid game name specified, these characters are not allowed: / ? * : ; { } \\'));
    return;
  }
  $exists_id = mg_game_name_exists($element['#value']);
  if ($form_state['values']['gid'] != 0 && $form_state['values']['gid'] != $exists_id && $exists_id != 0) {
    form_error($element, t('The specified game name already exists for another game.'));
  }
  else if ($form_state['values']['gid'] == 0 && $exists_id != 0) {
    form_error($element, t('The specified game name already exists for another game.'));
  }
}

function mg_is_valid_youtube_id($element, &$form_state) {
  if (strlen($element['#value']) != 11 ) {
    form_error($element, t('The specified Youtube video ID must be 11 characters'));
    return;
  }
  if (preg_match('/[^ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-]/', $element['#value'])) {
    form_error($element, t('The specified Youtube video ID contains invalid characters'));
    return;
  }
}

/**
 * Removes all form errors and messages for specified form element
 * http://api.drupal.org/api/drupal/includes%21form.inc/function/form_get_errors/7#comment-28464
 * @param type $element_name
 * @param type $form
 * @param type $form_state
 */
function mg_remove_form_element_errors($element_name, $form, &$form_state) {

  $errors = form_get_errors();
  if ($errors) {
    // Clear errors.
    form_clear_error();
    // Clear error messages.
    $error_messages = drupal_get_messages('error');
    // Initialize an array where removed error messages are stored.
    $removed_messages = array();

    // Remove all errors originated by the 'foo][bar' element.
    foreach ($errors as $name => $error_message) {
      if ($name == $element_name) {
        $removed_messages[] = $error_message;
        unset($errors[$name]);
      }
    }

    // Reinstate remaining errors.
    foreach ($errors as $name => $error) {
      form_set_error($name, $error);
      // form_set_error() calls drupal_set_message(), so we have to filter out
      // these from the error messages as well.
      $removed_messages[] = $error;
    }

    // Reinstate remaining error messages (which, at this point, are messages that
    // were originated outside of the validation process).
    foreach (array_diff($error_messages['error'], $removed_messages) as $message) {
      drupal_set_message($message, 'error');
    }
  }
}