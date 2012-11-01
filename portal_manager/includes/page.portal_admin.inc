<?php

/**
 * @file
 * Content for page with portal admin functions
 */
function portal_admin_page() {
 
  $breadcrumbs = array(
      l(t('Home'), '<front>')
      );
  drupal_set_breadcrumb($breadcrumbs);
  
  if ( subdomain()->name() == '' ) {
    $options = array(
        l('Subdomains', 'portal/admin/subdomains/'), 
        l('Games', 'portal/admin/games/'));
  }
 else {
    $options = array(
        l('Subdomains', 'portal/admin/subdomains/'), 
        l('Games', 'portal/admin/games/'),
        l('Create a subdomain', 'portal/admin/subdomains/create'), 
        l('Edit this subdomain', 'portal/admin/subdomains/edit/'.subdomain()->name()));
  }
  
  global $user;
  if (in_array('administrator', $user->roles)) {
    $options[] = l('Test Data', 'portal/admin/testdata/');
  }
          
  
  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => '<p>Portal admin functions</p>',
      ),
      'second_para' => array(
          '#items' => $options,
          '#theme' => 'item_list',
      ),
  );
  return $output;

}

?>
