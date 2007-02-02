<?php
  include('../dbfiles/dbms.data');

  $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
      or die("Could not connect");

  mysql_select_db($mysql_dbms) or die("Could not select database");

  /* Okay, see if they submitted anything: */
  if ($_POST['l_verified'] != "") {
    /* create a MySQL insert statement: */
    $ld_enabled=(int)$_POST['l_enabled'];
    $ld_worked=(int)$_POST['l_worked'];
    $ld_mtxversion=mysql_escape_string($_POST['l_mtxversion']);
    $ld_osname=mysql_escape_string($_POST['l_osname']);
    $ld_osversion=mysql_escape_string($_POST['l_osversion']);
    $ld_description=mysql_escape_string($_POST['l_description']);
    $ld_vendorid=mysql_escape_string($_POST['l_vendorid']);
    $ld_productid=mysql_escape_string($_POST['l_productid']);
    $ld_revision=mysql_escape_string($_POST['l_revision']);
    $ld_barcodes=mysql_escape_string($_POST['l_barcodes']);
    $ld_eaap=mysql_escape_string($_POST['l_eaap']);
    $ld_transports=(int)$_POST['l_transports'];
    $ld_slots=(int)$_POST['l_slots'];
    $ld_transfers=(int)$_POST['l_transfers'];
    $ld_imports=(int)$_POST['l_imports'];
    $ld_tgdp=mysql_escape_string($_POST['l_tgdp']);
    $ld_canxfer=mysql_escape_string($_POST['l_canxfer']);
    $ld_serialnum=mysql_escape_string($_POST['l_serialnum']);
    $ld_email=mysql_escape_string($_POST['l_email']);
    $ld_name=mysql_escape_string($_POST['l_name']);
    $ld_comments=mysql_escape_string($_POST['l_comments']);
    /* 'contributed' is calculated by 'now()' function */

    $query_str="insert into loaders (enabled,worked,osname,osversion,mtxversion,description,vendorid,productid,revision,barcodes,eaap,transports,slots,imports,transfers,tgdp,canxfer,serialnum,email,name,contributed,comments) values ( 1, 1, '$ld_osname', '$ld_osversion', '$ld_mtxversion', '$ld_description', '$ld_vendorid', '$ld_productid', '$ld_revision', $ld_barcodes, $ld_eaap, $ld_transports, $ld_slots, $ld_imports, $ld_transfers, $ld_tgdp, $ld_canxfer, '$ld_serialnum', '$ld_email', '$ld_name', now(), '$ld_comments') ";

    /* now to insert it: */

    $result=mysql_query($query_str,$link);

    /* Now send mail to Eric telling him that it's been added.  */
    /* mail("eric@badtux.org","New Addition to MTX Compatibility List",$query_str); */
    /* Now to go back to index.html: */
    header("Location: http://mtx.sourceforge.net/");
    exit();  
  }
?>
<!DOCTYPE html PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>
    <title>Entry Verification</title>
    <style type="text/css">
      <!--
        TH { background-color: cyan; text-align: right; }
        TR { background-color: white; }
        H2 { text-align: center; }
        BODY { background-color: white; }
        A:link { color: blue; text-decoration: underline; }
        A:active { text-decoration: underline; }
        A:hover { text-decoration: underline; }
        A:visited { color: blue; text-decoration: underline; }
      -->
    </style>
  </head>

  <body>
    <a href="http://sourceforge.net">
      <img src="http://sflogo.sourceforge.net/sflogo.php?group_id=4626&amp;type=7" width="210" height="62" border="0" style="position:absolute;left:0.125in;" alt="SourceForge.net Logo"/>
    </a>
    <h2>Compatibility Entry Verification</h2>
    <br clear="left"/>
    <br/>
    You entered the following values. If they are correct, click on "Save".
    If they are not correct, hit the BACK button on your browser:

    <form action="verify.php" method="POST">
      <input type="hidden" name="l_verified" value="1"/>
      <?php echo '<input type="hidden" name="l_enabled" value="',$_POST['l_enabled'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_worked" value="',$_POST['l_worked'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_osname" value="',$_POST['l_osname'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_osversion" value="',$_POST['l_osversion'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_mtxversion" value="',$_POST['l_mtxversion'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_description" value="',$_POST['l_description'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_vendorid" value="',$_POST['l_vendorid'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_productid" value="',$_POST['l_productid'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_revision" value="',$_POST['l_revision'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_barcodes" value="',$_POST['l_barcodes'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_eaap" value="',$_POST['l_eaap'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_transports" value="',$_POST['l_transports'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_slots" value="',$_POST['l_slots'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_transfers" value="',$_POST['l_transfers'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_imports" value="',$_POST['l_imports'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_tgdp" value="',$_POST['l_tgdp'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_canxfer" value="',$_POST['l_canxfer'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_serialnum" value="',$_POST['l_serialnum'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_email" value="',$_POST['l_email'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_name" value="',$_POST['l_name'],'"/>' ?>
      <?php echo '<input type="hidden" name="l_comments" value="',$_POST['l_comments'],'"/>' ?>
      <center>
        <table border="1">
          <tr>
            <th style="text-align: center" colspan="4">OS and General Info</th>
          </tr>
          <tr>
            <th>Operating System:</th>
            <td>
              <?php echo $_POST['l_osname'] ?>
            </td>
            <th>OS Version:</th>
            <td>
              <?php echo $_POST['l_osversion'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <th>MTX Version:</th>
            <td>
              <?php echo $_POST['l_mtxversion'] ?>
            </td>
            <th colspan="2">
              <br/>
            </th>
          </tr>
          <tr>
            <th>Loader Description:</th>
            <td colspan="3">
              <?php echo $_POST['l_description'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <th style="text-align: center" colspan="4">LoaderInfo Output</th>
          </tr>
          <tr>
            <th>Vendor ID:</th>
            <td>
              <?php echo $_POST['l_vendorid'],'<br/>' ?>
            </td>
            <th>Product ID:</th>
            <td>
              <?php echo $_POST['l_productid'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <th>Revision:</th>
            <td>
              <?php echo $_POST['l_revision'],'<br/>' ?>
            </td>
            <th>Serial Number:</th>
            <td>
              <?php echo $_POST['l_serialnum'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <th>Barcode Reader:</th>
            <td>
              <?php echo $_POST['l_barcodes'] == 1 ? "Yes" : "No" ?>
            </td>
            <th>Element Address Assignment Page:</th>
            <td>
              <?php echo $_POST['l_eaap'] == 1 ? "Yes" : "No" ?>
            </td>
          </tr>
          <tr>
            <th>Transfer Geometry Descriptor Page:</th>
            <td>
              <?php echo $_POST['l_tgdp'] == 1 ? "Yes" : "No" ?>
            </td>
            <th>Can Transfer:</th>
            <td>
              <?php echo $_POST['l_canxfer'] == 1 ? "Yes" : "No" ?>
            </td>
          </tr>
          <tr>
            <th>Number of Medium Transport Elements:</th>
            <td>
              <?php echo $_POST['l_transports'] ?>
            </td>
            <th>Number of Storage Elements:</th>
            <td>
              <?php echo $_POST['l_slots'] ?>
            </td>
          </tr>
          <tr>
            <th>Number of Import/Export Elements:</th>
            <td>
              <?php echo $_POST['l_imports'] ?>
            </td>
            <th>Number of Data Transfer Elements:</th>
            <td>
              <?php echo $_POST['l_transfers'] ?>
            </td>
          </tr>
          <tr>
            <th style="text-align: center" colspan="4">Comments</th>
          </tr>
          <tr>
            <td colspan="4">
              <?php echo $_POST['l_comments'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <th style="text-align: center" colspan="4">Personal Data</th>
          </tr>
          <tr>
            <th>Your Name:</th>
            <td>
              <?php echo $_POST['l_name'],'<br/>' ?>
            </td>
            <th>Your EMAIL Address: </th>
            <td>
              <?php echo $_POST['l_email'],'<br/>' ?>
            </td>
          </tr>
          <tr>
            <td colspan="2" align="right">
              <input type="submit" name="Save" value="Save"/>
            </td>
            <td colspan="2">
              <input type="button" name="Cancel" value="Cancel" onClick="history.go(-1)"/>
            </td>
          </tr>
        </table>
      </center>
    </form>
  </body>
</html>
