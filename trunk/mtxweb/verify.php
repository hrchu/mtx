<?php

   include('dbms.data');

   $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
        or die("Could not connect");
   mysql_select_db($mysql_dbms) or die("Could not select database");

  /* Okay, see if they submitted anything: */
  if ( "$l_verified" != "" ) {
     /* create a MySQL insert statement: */
     $ld_enabled=(int)$l_enabled;
     $ld_worked=(int)$l_worked;
     $ld_mtxversion=mysql_escape_string($l_mtxversion);
     $ld_osname=mysql_escape_string($l_osname);
     $ld_osversion=mysql_escape_string($l_osversion);
     $ld_description=mysql_escape_string($l_description);
     $ld_vendorid=mysql_escape_string($l_vendorid);
     $ld_productid=mysql_escape_string($l_productid);
     $ld_revision=mysql_escape_string($l_revision);
     $ld_barcodes=mysql_escape_string($l_barcodes);
     $ld_eaap=mysql_escape_string($l_eaap);
     $ld_transports=(int)$l_transports;
     $ld_slots=(int)$l_slots;
     $ld_transfers=(int)$l_transfers;
     $ld_imports=(int)$l_imports;
     $ld_tgdp=mysql_escape_string($l_tgdp);
     $ld_canxfer=mysql_escape_string($l_canxfer);
     $ld_serialnum=mysql_escape_string($l_serialnum);
     $ld_email=mysql_escape_string($l_email);
     $ld_name=mysql_escape_string($l_name);
     $ld_comments=mysql_escape_string($l_comments);
      /* 'contributed' is calculated by 'now()' function */

     if ($ld_canxfer=="Yes") {
        $ld_canxfer=1;
     } else {
        $ld_canxfer=0;
     }

     if ($ld_eaap=="Yes") {
       $ld_eaap=1;
     } else {
        $ld_eaap=0;
     }	 
     if ($ld_tgdp=="Yes") {
	$ld_tgdp=1;
     } else {
        $ld_tgdp=0;
     }
     if ($ld_barcodes=="Yes") {
        $ld_barcodes=1;
     } else { 
        $ld_barcodes=0;
     } 

     $query_str="insert into loaders (enabled, worked,osname,osversion,mtxversion,description,vendorid,productid,revision,barcodes,eaap,transports,slots,imports,transfers,tgdp,canxfer,serialnum,email,name,contributed,comments) values ( 1, 1, '$ld_osname', '$ld_osversion', '$ld_mtxversion', '$ld_description', '$ld_vendorid', '$ld_productid', '$ld_revision', $ld_barcodes, $ld_eaap, $ld_transports, $ld_slots, $ld_imports, $ld_transfers, $ld_tgdp, $ld_canxfer, '$ld_serialnum', '$ld_email', '$ld_name', now(), '$ld_comments') ";
 
    /* now to insert it: */
    
    $result=mysql_query($query_str,$link);

    /* Now send mail to Eric telling him that it's been added.  */
    mail("eric@badtux.org","New Addition to MTX Compatibility List",$query_str);
    /* Now to go back to index.html: */
     header("Location: http://mtx.badtux.net/");
     exit();  

  }?> <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
  <head>

    <title>Entry Verification</title>

<style type="text/css">
  <!--
    TH { bgcolor: cyan;  }
    TR { background-color: white; }
    H1 { text-align: center; }
    BODY { background-color: white; }
     A:link { color: blue; text-decoration: none ; }

     A:active { text-decoration: underline ; }
     A:hover { text-decoration: underline ; }

     A:visited { color: violet; text-decoration: none ; }
  -->
</style>
  </head>

<body bgcolor="white">
<center><h1>Compatibility Entry Verification</h1></center>
You entered the following values. If they are correct, click on "Save". 
If they are not correct, hit the BACK button on your browser:

<form action="verify.php" method="POST">
<input type="hidden" name="l_verified" value="1">
<input type="hidden" name="l_enabled" value="<?php print "$l_enabled"?>">
<input type="hidden" name="l_worked" value="<?php print "$l_enabled"?>">
<input type="hidden" name="l_osname" value="<?php print "$l_osname"?>">
<input type="hidden" name="l_osversion" value="<?php print "$l_osversion"?>">
<input type="hidden" name="l_mtxversion" value="<?php print "$l_mtxversion"?>">
<input type="hidden" name="l_description" value="<?php print "$l_description"?>">
<input type="hidden" name="l_vendorid" value="<?php print "$l_vendorid"?>">
<input type="hidden" name="l_productid" value="<?php print "$l_productid"?>">
<input type="hidden" name="l_revision" value="<?php print "$l_revision"?>">
<input type="hidden" name="l_barcodes" value="<?php print "$l_barcodes"?>">
<input type="hidden" name="l_eaap" value="<?php print "$l_eaap"?>">
<input type="hidden" name="l_transports" value="<?php print "$l_transports"?>">
<input type="hidden" name="l_slots" value="<?php print "$l_slots"?>">
<input type="hidden" name="l_transfers" value="<?php print "$l_transfers"?>">
<input type="hidden" name="l_imports" value="<?php print "$l_imports"?>">
<input type="hidden" name="l_tgdp" value="<?php print "$l_tgdp"?>">
<input type="hidden" name="l_canxfer" value="<?php print "$l_canxfer"?>">
<input type="hidden" name="l_serialnum" value="<?php print "$l_serialnum"?>">
<input type="hidden" name="l_email" value="<?php print "$l_email"?>">
<input type="hidden" name="l_name" value="<?php print "$l_name"?>">
<input type="hidden" name="l_comments" value="<?php print "$l_comments"?>">

<table border=1>
<tr>
   <th colspan=4 bgcolor="cyan">
         OS and General Info
    </th>
</tr>      
<tr>
    <!-- do a pulldown for operating system name:  -->
   <th align="right" bgcolor="cyan">Operating System:   </th>
    </th>          
    <td> <?php print "$l_osname" ?> </td>
     <th align="right" bgcolor="cyan">OS Version: </th>
     <td>  <?php print "$l_osversion" ?> </td>
   </tr>
   <tr>
      <th align="right" bgcolor="cyan">MTX Version: </th>
      <td> <?php print "$l_mtxversion" ?> </td> 
   </tr>
   <tr> 
     <th align="right" bgcolor="cyan"> Loader Description:</th>
     <td colspan=3>
         <?php print "$l_description" ?>
     </td>
   </tr>
   <tr>
      <th colspan=4 bgcolor="cyan">TapeInfo output</th>
   </tr>
   <tr>
       <th align="right" bgcolor="cyan">Vendor ID: </th>
       <td> <?php print "$l_vendorid" ?> </td>
       <th align="right" bgcolor="cyan">Product ID: </th>
          <td> <?php print "$l_productid" ?> </td>
    </tr>
    <tr>
       <th align="right" bgcolor="cyan">Revision: </th>
       <td> <?php print "$l_revision"; ?> </td>
       <th align="right" bgcolor="cyan">SerialNumber: </th>
       <td> <?php print "$l_serialnum"; ?> </td>
    </tr>
    <tr>
       <th colspan=4 bgcolor="cyan">LoaderInfo output</th>
    </tr>
    <tr>
    <th align="right" bgcolor="cyan">Barcode Reader: </th>
    <td> <?php print "$l_barcodes" ?> </td>
     <th align="right" bgcolor="cyan">Element Address Assignment Page: </th>
    <td>  <?php print "$l_eaap" ?> </td>
   </tr>     
   <tr>
   <th align="right" bgcolor="cyan">Transfer Geometry Descriptor Page: </th>
    <td> <?php print "$l_tgdp" ?> 
     </td>
   <th align="right" bgcolor="cyan">Can Transfer: </th>
    <td> <?php print "$l_canxfer" ?> 
     </td>
  </tr>
  <tr> 
   <th align="right" bgcolor="cyan">Number of Medium Transport Elements: </th>
   <td> <?php print "$l_transports"; ?> </td>
   <th align="right" bgcolor="cyan">Number of Storage Elements: </th>       
   <td> <?php print "$l_slots"; ?> </td>
  </tr>
  <tr>
   <th align="right" bgcolor="cyan">Number of Import/Export Elements: </th> 
   <td> <?php print "$l_imports"; ?> </td>  
   <th align="right" bgcolor="cyan">Number of Data Transfer Elements: </th> 
   <td> <?php print "$l_transfers"; ?> </td>  
  </tr>
  <tr>
    <th colspan=4>Personal Data</a>
  </tr>
  <tr>
   <th align="right" bgcolor="cyan">Your Name: </th> 
   <td> <?php print "$l_name"; ?> </td>  
   <th align="right" bgcolor="cyan">Your EMAIL Address: </th> 
   <td> <?php print "$l_email"; ?>  </td>     
  </tr>
  <tr>
     <th colspan=4>Comments</a>
  </tr>
  <tr>
     <td colspan=4"> <?php print "$l_comments"; ?> </td> 
  </tr>     
  <tr>
    <td colspan=4 align="center">
       <input type="submit" name="Save" value="Save">
    </td>
    </form>
  </tr>
</table>

