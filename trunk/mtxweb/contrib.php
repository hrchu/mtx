<?php
   include('dbms.data');

   $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
        or die("Could not connect");
   mysql_select_db($mysql_dbms) or die("Could not select database");

   
?><!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>

    <title>Contribute to MTX compatibility list</title>

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
<h1> Contribute to MTX Compatibility List </h1>

WARNING: This application is currently in beta test, and may be buggy. 
Please only use it to submit entries that you have verified as being
compatible with 'mtx'.  <b>Please note that your EMAIL address will *NOT*
be published, and will be used only in the event that I have questions about
your entry. </b>
<p>
You will need the following information:
<ol> 
<li> Your operating system type and version number (e.g. "cat /proc/version" 
on Linux, or "uname -v" on FreeBSD)
<li> Your MTX version ( mtx --version )
<li> The result of 'tapeinfo' on your loader
<li> The result of 'loaderinfo' on your loader
</ol> 

Please note that the 'barcode' output from 'loaderinfo' is not accurate for
most loaders. Please report whether barcodes actually show up when you do
'mtx status'. 

<p>
<form action="verify.php" method="POST">
<input type="hidden" name="l_enabled" value="1">
<input type="hidden" name="l_worked" value="1">

<!-- now for the table: -->
<table border=1 width="100%">
<tr>
   <th colspan=4 bgcolor="cyan">
         OS and General Info
    </th>
</tr>      
<tr>
    <!-- do a pulldown for operating system name:  -->
   <th align="right" bgcolor="cyan">Operating System:   
   <?php 
         $query_str="select osname from hosts group by osname order by osname";
         $result=mysql_query($query_str,$link) or die("</th></tr></table>Invalid query string '$query_str'");
    ?>
    </th>          
    <td> <select name="l_osname">
         <?php 
            while ($row=mysql_fetch_assoc($result)) {
               extract($row);
               if ("$osname" == "$l_osname") {
	           $selected="SELECTED";
               } else { 
                   $selected="";
               }
               print("<option value='$osname' $selected >$osname</option>");
            }
          ?> 
         </select>
     </td>
     <th align="right" bgcolor="cyan">OS Version: </th>
     <td>
      <input name="l_osversion" type="text" value="<?php print "$l_osversion"; ?>" size="40">
     </td>
   </tr>
   <tr>
     <th align="right" bgcolor="cyan">MTX Version (mtx --version):</th>
     <td>
       <input name="l_mtxversion" type="text" value="<?php print "$l_mtxversion"; ?>" size="15">
     </td>
   <tr>
   <tr> 
     <th align="right" bgcolor="cyan"> Loader Description:</th>
     <td colspan=3>
         <input name="l_description" type="text" value="<?php print "$l_description"; ?>" size="80">
     </td>
   </tr>
   <tr>
      <th colspan=4 bgcolor="cyan">TapeInfo output</th>
   </tr>
   <tr>
       <th align="right" bgcolor="cyan">Vendor ID: </th>
       <td> <input name="l_vendorid" type="text" value="<?php print "$l_vendorid"; ?>" size="40"> </td>
       <th align="right" bgcolor="cyan">Product ID: </th>
          <td> <input name="l_productid" type="text" value="<?php print "$l_productid"; ?>" size="40"> </td>
    </tr>
    <tr>
       <th align="right" bgcolor="cyan">Revision: </th>
       <td> <input name="l_revision" type="text" value="<?php print "$l_revision"; ?>" size="40"> </td>
       <th align="right" bgcolor="cyan">SerialNumber: </th>
       <td> <input name="l_serialnum" type="text" value="<?php print "$l_serialnum"; ?>" size="40"> </td>
    </tr>
    <tr>
       <th colspan=4 bgcolor="cyan">LoaderInfo output</th>
    </tr>
    <tr>
    <th align="right" bgcolor="cyan">Barcode Reader: </th>
    <td> <select name="l_barcodes">
         <option value="Yes" <?php if ($ld_barcodes==1) { print "SELECTED"; }  ?> >Yes</a>
         <option value="No" <?php if ($ld_barcodes==0) { print "SELECTED"; } ?> >No</a>
         </select>
    </td>
     <th align="right" bgcolor="cyan">Element Address Assignment Page (EAAP): </th>
    <td> <select name="l_eaap">
         <option value="Yes" <?php if ($ld_eaap==1) { print "SELECTED"; } ?> >Yes</a>
         <option value="No" <?php if ($ld_eaap==0) { print "SELECTED"; } ?> >No</a>
         </select>
     </td>
   </tr>     
   <tr>
   <th align="right" bgcolor="cyan">Transfer Geometry Descriptor Page: </th>
    <td> <select name="l_tgdp">
         <option value="Yes" <?php if ($ld_tgdp==1) { print "SELECTED"; } ?> >Yes</a>
         <option value="No" <?php if ($ld_tgdp==0) { print "SELECTED"; } ?> >No</a>
         </select>
     </td>
   <th align="right" bgcolor="cyan">Can Transfer: </th>
    <td> <select name="l_canxfer">
         <option value="Yes" <?php if ($ld_canxfer==1) { print "SELECTED"; } ?> >Yes</a>
         <option value="No" <?php if ($ld_canxfer==0) { print "SELECTED"; } ?> >No</a>
         </select>
     </td>
  </tr>
  <tr> 
   <th align="right" bgcolor="cyan">Number of Medium Transport Elements: </th>
   <td> <input name="l_transports" type="text" value="<?php print "$l_transports"; ?>" size=15> </td>
   <th align="right" bgcolor="cyan">Number of Storage Elements: </th>       
   <td> <input name="l_slots" type="text" value="<?php print "$l_slots"; ?>" size=15> </td>
  </tr>
  <tr>
   <th align="right" bgcolor="cyan">Number of Import/Export Elements: </th> 
   <td> <input name="l_imports" type="text" value="<?php print "$l_imports"; ?>" size=15> </td>  
 
   <th align="right" bgcolor="cyan">Number of Data Transfer Elements: </th> 
   <td> <input name="l_transfers" type="text" value="<?php print "$l_transfers"; ?>" size=15> </td>  
  </tr>
  <tr>
    <th colspan=4>Personal Data</a>

  </tr>
  <tr>
   <th align="right" bgcolor="cyan">Your Name: </th> 
   <td> <input name="l_name" type="text" value="<?php print "$l_name"; ?>" size=30> </td>  
   <th align="right" bgcolor="cyan">Your EMAIL Address: </th> 
   <td> <input name="l_email" type="text" value="<?php print "$l_email"; ?>" size=30> </td>     
  </tr>
  <tr>
     <th colspan=4>Comments</a>
  </tr>
  <tr>
     <td colspan=4 align="center"> <textarea name="l_comments" cols="70" rows="4" wrap="virtual"><?php print "$l_comments"; ?></textarea> </td> 
  </tr>     
  <tr>
    <td colspan=2 align="right">
       <input type="submit" name="Save" value="Save">
     </td> </form>
     <form action="index.html" method="GET"> 
     <td>
     <input type="submit" name="Cancel" value="Cancel">
    </td>
    </form>
  </tr>
</table>

       
</body>
</html>


