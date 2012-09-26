<?php

/**
 * @file
 * Contains the form for editing and creating a subdomain.
 */
function subdomain_edit_form($form, &$form_state, $subdomain = '') {
  global $theme_key;
  $defaults = array('name' => '', 'group_name' => '', 'theme_name' => $theme_key, 'sdid' => 0);
  if (!empty($subdomain)) {
    $subdomain = filter_xss($subdomain);
    if (preg_match('/^[a-z\d\-]+$/i', $subdomain)) {
      $defaults = subdomain()->get_array($subdomain);
      $form['lastedit'] = array(
          '#type' => 'item',
          //'#title' => t('Last edited on'),
          '#markup' =>t('This subdomain was last edited on @date', array('@date' => format_date($defaults['updated_on'])))
      );
      if (!$defaults) {
        drupal_set_message(t('The specified subdomain has not been configured, you can configure it now'), 'error');
        $defaults = array('name' => $subdomain, 'theme_name' => $theme_key, 'group_name' => '', 'sdid' => 0);
        
      }
    } else {
      drupal_set_message(t('An invalid subdomain was specified'), 'error');
    }
  }

  // get all enabled themes
  $all_themes = list_themes();
  $theme_list = array();
  foreach ($all_themes as $theme_name => $theme)
  {
    if (drupal_theme_access($theme)) {
      $theme_list[$theme_name] = $theme->info['name'];
    }
  }
  
  $form['sdid'] = array('#type' => 'value', '#value' => $defaults['sdid']);

  $form['subdomain_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Subdomain name'),
      '#description' => t('The name of the subdomain in the url. For example in "http://group1.microgame.nl" the subdomain name is "group1".'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
      '#default_value' => $defaults['name'],
      '#element_validate' => array('mg_is_valid_domain_name'),
  );

  $form['subdomain_group_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Subdomain group'),
      '#description' => t('The name of the group or organization that will make use of this subdomain.'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
      '#default_value' => $defaults['group_name']
  );
  
   $form['subdomain_theme_name'] = array(
       '#type'          => 'select',
       '#title'         => t('Theme to use for this subdomain'),
       '#options'       => $theme_list,
       '#default_value' => $defaults['theme_name']
    );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
  );

  return $form;
}

function subdomain_edit_form_submit($form, &$form_state) {
  global $user;
  if ($form_state['values']['sdid'] == 0) {
    // create a new subdomain
    db_insert('microgame_subdomains')
            ->fields(array(
                'name' => $form_state['values']['subdomain_name'],
                'group_name' => $form_state['values']['subdomain_group_name'],
                'theme_name' =>$form_state['values']['subdomain_theme_name'],
                'created_by_uid' => $user->uid,
                'updated_by_uid' => $user->uid,
                'created_on' => REQUEST_TIME,
                'updated_on' => REQUEST_TIME,
            ))
            ->execute();
    drupal_set_message(t('Subdomain "@name" has been created', array('@name' => $form_state['values']['subdomain_name'])));
  } else {
    // update an existing subdomain    
    db_update('microgame_subdomains')
            ->fields(array(
                'name' => $form_state['values']['subdomain_name'],
                'group_name' => $form_state['values']['subdomain_group_name'],
                'theme_name' =>$form_state['values']['subdomain_theme_name'],
                'updated_by_uid' => $user->uid,
                'updated_on' => REQUEST_TIME,
            ))
            ->condition('sdid', $form_state['values']['sdid'], '=')
            ->execute();
    drupal_set_message(t('Subdomain "@name" with id @id has been updated', array('@name' => $form_state['values']['subdomain_name'], '@id' => $form_state['values']['sdid'])));
  }
  drupal_goto('portal/admin/subdomains');
}

function mg_is_valid_domain_name($element, &$form_state) {
  $a=1;
  if (!preg_match('/^[a-z\d\-]+$/i', $element['#value'])) {
    form_error($element, t('Invalid name specified, only alphanumeric characters and the dash are allowed'));
  } 
  else if ($form_state['values']['sdid'] == 0 && subdomain()->name_exists($element['#value'])) {
    form_error($element, t('The specified name already exists'));
  }
  else if ($form_state['values']['sdid'] != 0 && subdomain()->name_exists($element['#value'])  && subdomain()->name_exists($element['#value']) != $form_state['values']['sdid'] ) {
    form_error($element, t('The specified name is already in use'));
  }
}