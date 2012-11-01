<?php

/**
 * @file
 * Contains the form for editing and uploading a game.
 */
function testdata_form($form, &$form_state, $gid = 0) {
  menu_tree_set_path('microgame_portal', 'portal/admin');

  $breadcrumbs = array(
      l(t('Home'), '<front>'),
      l(t('Configuration'), 'portal/admin'),
  );
  drupal_set_breadcrumb($breadcrumbs);
  
  $defaults = array('name' => '', 'description' => '', 'gid' => 0, 'youtube_id' => '');
  
  $form['min_per_day'] = array(
      '#type' => 'textfield',
      '#title' => t('Minimum score submits per day'),
      '#description' => t('Minimum number of submits each day'),
      '#size' => 5,
      '#maxlength' => 5,
      '#required' => TRUE,
      '#default_value' => 50,
  );
  
  $form['max_per_day'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum score submits per day'),
      '#description' => t('Maximum number of submits each day'),
      '#size' => 5,
      '#maxlength' => 5,
      '#required' => TRUE,
      '#default_value' => 5000,
  );
  // we generate random values according to buckets
  // each bucket is a time period in hours that has a chance for generating a value
  // the size of the bucket is determined by the hours it encompasses
  // the chance for a value is determined by the chance given and the size of the bucket
  // we then generate random values between 0 and the total bucket size
  // the value determines which bucket is lucky
  // we then draw another random value from that bucket
  
  // create list of buckets by start hour for timestamp generation
  // this generates buckets for 0-2 hours, 2-7 hours etc
  $bucketlist = array(0,2,7,9,12,15,22);
  $chance = array(5,3,15,18,24,25,10);
  $count = count($bucketlist);
  
  // generate buckets
  // each bucket "to" is the size (next in bucketlist -current bucketlist entry) * chance it has + previous buckets
  $buckets = array();
  for ($i=0; $i < $count; $i++) {    
    $buckets[$i]['from'] = $bucketlist[$i];
    $buckets[$i]['to'] = $i == $count-1 ? 24 : $bucketlist[$i+1];
    $buckets[$i]['fromsize'] = $i == 0 ? 0 : $buckets[$i-1]['tosize'];
    $buckets[$i]['tosize'] = ($buckets[$i]['to'] - $bucketlist[$i]) * ($chance[$i]/100) + $buckets[$i]['fromsize'] ;
    //$buckets[$i]['size'] = $buckets[$i]['tosize'] - $buckets[$i]['fromsize'];
  }
  $total_bucket_size = $buckets[$i-1]['tosize'];
  
//  foreach ($buckets as $b ) {
//    $output .= "{$b['from']}-{$b['to']} - {$b['size']}<br>";
//  }
  
  $output =  '';
  for ($i=0; $i < 1000; $i++) {
    // generate a random float between 0 and total bucket size
    $rand = mt_rand(0,$total_bucket_size*100)/100;
    // find the lucky bucket
    foreach ($buckets as $b){
      if ($rand <= $b['tosize']) {
         // generate a random number of minutes within bucket range
        $total_minute_range = ($b['to'] - $b['from']) *60;
        $b_rand = mt_rand(0,$total_minute_range);
        // convert minutes back to hour/minutes on 24h clock
        $hour = $b['from'] + floor ($b_rand/60);
        $minutes = $b_rand % 60;
        break;
      }
    }

    $output .= "{$hour}:{$minutes}<br>" ;
  }
  
  $form['test'] = array(
      '#markup' => $output
  );
  
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
  );

  return $form;
}










// http://stackoverflow.com/questions/3109670/generate-random-numbers-with-probabilistic-distribution



//
//function mkseed()
//{
//    srand(hexdec(substr(md5(microtime()), -8)) & 0x7fffffff) ;
//}   //  function mkseed()
//
//
//
//
///*
//function factorial($in) {
//    if ($in == 1) {
//        return $in ;
//    }
//    return ($in * factorial($in - 1.0)) ;
//}   //  function factorial()
//
//
//function factorial($in) {
//    $out = 1 ;
//    for ($i = 2; $i <= $in; $i++) {
//        $out *= $i ;
//    }
//
//    return $out ;
//}   //  function factorial()
//*/
//
//
//
//
//function random_0_1()
//{
//    //  returns random number using mt_rand() with a flat distribution from 0 to 1 inclusive
//    //
//    return (float) mt_rand() / (float) mt_getrandmax() ;
//}   //  random_0_1()
//
//
//function random_PN()
//{
//    //  returns random number using mt_rand() with a flat distribution from -1 to 1 inclusive
//    //
//    return (2.0 * random_0_1()) - 1.0 ;
//}   //  function random_PN()
//
//
//
//
//function gauss()
//{
//    static $useExists = false ;
//    static $useValue ;
//
//    if ($useExists) {
//        //  Use value from a previous call to this function
//        //
//        $useExists = false ;
//        return $useValue ;
//    } else {
//        //  Polar form of the Box-Muller transformation
//        //
//        $w = 2.0 ;
//        while (($w >= 1.0) || ($w == 0.0)) {
//            $x = random_PN() ;
//            $y = random_PN() ;
//            $w = ($x * $x) + ($y * $y) ;
//        }
//        $w = sqrt((-2.0 * log($w)) / $w) ;
//
//        //  Set value for next call to this function
//        //
//        $useValue = $y * $w ;
//        $useExists = true ;
//
//        return $x * $w ;
//    }
//}   //  function gauss()
//
//
//function gauss_ms( $mean,
//                   $stddev )
//{
//    //  Adjust our gaussian random to fit the mean and standard deviation
//    //  The division by 4 is an arbitrary value to help fit the distribution
//    //      within our required range, and gives a best fit for $stddev = 1.0
//    //
//    return gauss() * ($stddev/4) + $mean;
//}   //  function gauss_ms()
//
//
//function gaussianWeightedRandom( $LowValue,
//                                 $maxRand,
//                                 $mean=0.0,
//                                 $stddev=2.0 )
//{
//    //  Adjust a gaussian random value to fit within our specified range
//    //      by 'trimming' the extreme values as the distribution curve
//    //      approaches +/- infinity
//    $rand_val = $LowValue + $maxRand ;
//    while (($rand_val < $LowValue) || ($rand_val >= ($LowValue + $maxRand))) {
//        $rand_val = gauss_ms($mean,$stddev) * $maxRand + $LowValue ;
//        $rand_val = ($rand_val + $maxRand) / 2 ;
//    }
//
//    return $rand_val ;
//}   //  function gaussianWeightedRandom()
//
//
//function bellWeightedRandom( $LowValue, $maxRand )
//{
//    return gaussianWeightedRandom( $LowValue, $maxRand, 0.0, 1.0 ) ;
//}   //  function bellWeightedRandom()
//
//
//function gaussianWeightedRisingRandom( $LowValue,
//                                       $maxRand )
//{
//    //  Adjust a gaussian random value to fit within our specified range
//    //      by 'trimming' the extreme values as the distribution curve
//    //      approaches +/- infinity
//    //  The division by 4 is an arbitrary value to help fit the distribution
//    //      within our required range
//    $rand_val = $LowValue + $maxRand ;
//    while (($rand_val < $LowValue) || ($rand_val >= ($LowValue + $maxRand))) {
//        $rand_val = $maxRand - round((abs(gauss()) / 4) * $maxRand) + $LowValue ;
//    }
//
//    return $rand_val ;
//}   //  function gaussianWeightedRisingRandom()
//
//
//function gaussianWeightedFallingRandom( $LowValue,
//                                        $maxRand )
//{
//    //  Adjust a gaussian random value to fit within our specified range
//    //      by 'trimming' the extreme values as the distribution curve
//    //      approaches +/- infinity
//    //  The division by 4 is an arbitrary value to help fit the distribution
//    //      within our required range
//    $rand_val = $LowValue + $maxRand ;
//    while (($rand_val < $LowValue) || ($rand_val >= ($LowValue + $maxRand))) {
//        $rand_val = floor((abs(gauss()) / 4) * $maxRand) + $LowValue ;
//    }
//
//    return $rand_val ;
//}   //  function gaussianWeightedFallingRandom()
//
//
//function logarithmic($mean=1.0, $lambda=5.0)
//{
//    return ($mean * -log(random_0_1())) / $lambda ;
//}   //  function logarithmic()
//
//
//function logarithmicWeightedRandom( $LowValue,
//                                    $maxRand )
//{
//    do {
//        $rand_val = logarithmic() ;
//    } while ($rand_val > 1) ;
//
//    return floor($rand_val * $maxRand) + $LowValue ;
//}   //  function logarithmicWeightedRandom()
//
//
//function logarithmic10( $lambda=0.5 )
//{
//    return abs(-log10(random_0_1()) / $lambda) ;
//}   //  function logarithmic10()
//
//
//function logarithmic10WeightedRandom( $LowValue,
//                                      $maxRand )
//{
//    do {
//        $rand_val = logarithmic10() ;
//    } while ($rand_val > 1) ;
//
//    return floor($rand_val * $maxRand) + $LowValue ;
//}   //  function logarithmic10WeightedRandom()
//
//
//function gamma( $lambda=3.0 )
//{
//    $wLambda = $lambda + 1.0 ;
//    if ($lambda <= 8.0) {
//        //  Use direct method, adding waiting times
//        $x = 1.0 ;
//        for ($j = 1; $j <= $wLambda; $j++) {
//            $x *= random_0_1() ;
//        }
//        $x = -log($x) ;
//    } else {
//        //  Use rejection method
//        do {
//            do {
//                //  Generate the tangent of a random angle, the equivalent of
//                //      $y = tan(pi * random_0_1())
//                do {
//                    $v1 = random_0_1() ;
//                    $v2 = random_PN() ;
//                } while (($v1 * $v1 + $v2 * $v2) > 1.0) ;
//                $y = $v2 / $v1 ;
//                $s = sqrt(2.0 * $lambda + 1.0) ;
//                $x = $s * $y + $lambda ;
//            //  Reject in the region of zero probability
//            } while ($x <= 0.0) ;
//            //  Ratio of probability function to comparison function
//            $e = (1.0 + $y * $y) * exp($lambda * log($x / $lambda) - $s * $y) ;
//        //  Reject on the basis of a second uniform deviate
//        } while (random_0_1() > $e) ;
//    }
//
//    return $x ;
//}   //  function gamma()
//
//
//function gammaWeightedRandom( $LowValue,
//                              $maxRand )
//{
//    do {
//        $rand_val = gamma() / 12 ;
//    } while ($rand_val > 1) ;
//
//    return floor($rand_val * $maxRand) + $LowValue ;
//}   //  function gammaWeightedRandom()
//
//
//function QaDgammaWeightedRandom( $LowValue,
//                                 $maxRand )
//{
//    return round((asin(random_0_1()) + (asin(random_0_1()))) * $maxRand / pi()) + $LowValue ;
//}   //  function QaDgammaWeightedRandom()
//
//
//function gammaln($in)
//{
//    $tmp = $in + 4.5 ;
//    $tmp -= ($in - 0.5) * log($tmp) ;
//
//    $ser = 1.000000000190015
//            + (76.18009172947146 / $in)
//            - (86.50532032941677 / ($in + 1.0))
//            + (24.01409824083091 / ($in + 2.0))
//            - (1.231739572450155 / ($in + 3.0))
//            + (0.1208650973866179e-2 / ($in + 4.0))
//            - (0.5395239384953e-5 / ($in + 5.0)) ;
//
//    return (log(2.5066282746310005 * $ser) - $tmp) ;
//}   //  function gammaln()
//
//
//function poisson( $lambda=1.0 )
//{
//    static $oldLambda ;
//    static $g, $sq, $alxm ;
//
//    if ($lambda <= 12.0) {
//        //  Use direct method
//        if ($lambda <> $oldLambda) {
//            $oldLambda = $lambda ;
//            $g = exp(-$lambda) ;
//        }
//        $x = -1 ;
//        $t = 1.0 ;
//        do {
//            ++$x ;
//            $t *= random_0_1() ;
//        } while ($t > $g) ;
//    } else {
//        //  Use rejection method
//        if ($lambda <> $oldLambda) {
//            $oldLambda = $lambda ;
//            $sq = sqrt(2.0 * $lambda) ;
//            $alxm = log($lambda) ;
//            $g = $lambda * $alxm - gammaln($lambda + 1.0) ;
//        }
//        do {
//            do {
//                //  $y is a deviate from a Lorentzian comparison function
//                $y = tan(pi() * random_0_1()) ;
//                $x = $sq * $y + $lambda ;
//            //  Reject if close to zero probability
//            } while ($x < 0.0) ;
//            $x = floor($x) ;
//            //  Ratio of the desired distribution to the comparison function
//            //  We accept or reject by comparing it to another uniform deviate
//            //  The factor 0.9 is used so that $t never exceeds 1
//            $t = 0.9 * (1.0 + $y * $y) * exp($x * $alxm - gammaln($x + 1.0) - $g) ;
//        } while (random_0_1() > $t) ;
//    }
//
//    return $x ;
//}   //  function poisson()
//
//
//function poissonWeightedRandom( $LowValue,
//                                $maxRand )
//{
//    do {
//        $rand_val = poisson() / $maxRand ;
//    } while ($rand_val > 1) ;
//
//    return floor($x * $maxRand) + $LowValue ;
//}   //  function poissonWeightedRandom()
//
//
//function binomial( $lambda=6.0 )
//{
//}
//
//
//function domeWeightedRandom( $LowValue,
//                             $maxRand )
//{
//    return floor(sin(random_0_1() * (pi() / 2)) * $maxRand) + $LowValue ;
//}   //  function bellWeightedRandom()
//
//
//function sawWeightedRandom( $LowValue,
//                            $maxRand )
//{
//    return floor((atan(random_0_1()) + atan(random_0_1())) * $maxRand / (pi()/2)) + $LowValue ;
//}   //  function sawWeightedRandom()
//
//
//function pyramidWeightedRandom( $LowValue,
//                               $maxRand )
//{
//    return floor((random_0_1() + random_0_1()) / 2 * $maxRand) + $LowValue ;
//}   //  function pyramidWeightedRandom()
//
//
//function linearWeightedRandom( $LowValue,
//                               $maxRand )
//{
//    return floor(random_0_1() * ($maxRand)) + $LowValue ;
//}   //  function linearWeightedRandom()
//
//
//function nonWeightedRandom( $LowValue,
//                            $maxRand )
//{
//    return rand($LowValue,$maxRand+$LowValue-1) ;
//}   //  function nonWeightedRandom()
//
//
//
//
//function weightedRandom( $Method,
//                         $LowValue,
//                         $maxRand )
//{
//    switch($Method) {
//        case RandomGaussian         :
//            $rVal = gaussianWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomBell             :
//            $rVal = bellWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomGaussianRising   :
//            $rVal = gaussianWeightedRisingRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomGaussianFalling  :
//            $rVal = gaussianWeightedFallingRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomGamma            :
//            $rVal = gammaWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomGammaQaD         :
//            $rVal = QaDgammaWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomLogarithmic10    :
//            $rVal = logarithmic10WeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomLogarithmic      :
//            $rVal = logarithmicWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomPoisson          :
//            $rVal = poissonWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomDome             :
//            $rVal = domeWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomSaw              :
//            $rVal = sawWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomPyramid          :
//            $rVal = pyramidWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        case RandomLinear           :
//            $rVal = linearWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//        default                     :
//            $rVal = nonWeightedRandom( $LowValue, $maxRand ) ;
//            break ;
//    }
//
//    return $rVal;
//}
//





// http://php.net/manual/en/function.rand.php


function gauss()
{   // N(0,1)
    // returns random number with normal distribution:
    //   mean=0
    //   std dev=1
    
    // auxilary vars
    $x=random_0_1();
    $y=random_0_1();
    
    // two independent variables with normal distribution N(0,1)
    $u=sqrt(-2*log($x))*cos(2*pi()*$y);
    $v=sqrt(-2*log($x))*sin(2*pi()*$y);
    
    // i will return only one, couse only one needed
    return $u;
}

function gauss_ms($m=0.0,$s=1.0)
{   // N(m,s)
    // returns random number with normal distribution:
    //   mean=m
    //   std dev=s
    
    return gauss()*$s+$m;
}

function random_0_1()
{   // auxiliary function
    // returns random number with flat distribution from 0 to 1
    return (float)rand()/(float)getrandmax();
}
