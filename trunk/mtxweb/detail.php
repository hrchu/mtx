<?php
  include('../dbfiles/dbms.data');

  $record = $_GET['record'];
  
  $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
       or die("Could not connect");
  mysql_select_db($mysql_dbms) or die("Could not select database");

  $query_str="select * from loaders where id = $record";
  $result=mysql_query($query_str,$link) or die("Invalid query '$query_str'");
  $num_rows=mysql_num_rows($result);
?>
<!DOCTYPE html PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>
    <meta http-equiv="Pragma" content="no-cache"/>
    <title>MTX Compatibility List - Details</title>

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
    <h2>MTX Compatibility List<br/>Details</h2>
    <br clear="left" />
    <br/>
This application is currently in beta test, and may be buggy. The database
that it feeds off of is currently very incomplete. Please 
<a href="contrib.php">contribute</a> new entries so that others can
benefit. Also see the <a href="COMPATIBILITY.html">old compatibility list</a>.
    <p/>
    <p/>
    <table align="center" border="1">
      <tr>
        <th style="text-align: center" colspan="4">OS and General Info</th>
      </tr>
<?php 
  if ($num_rows == 1 && ($row = mysql_fetch_assoc($result))) {
    extract($row);
?>
      <tr>
        <th>Operating System:</th>
        <td>
          <?php echo "$osname" ?>
        </td>
        <th>OS Version:</th>
        <td>
          <?php echo "$osversion<br/>" ?>
        </td>
      </tr>
      <tr>
        <th>MTX Version:</th>
        <td>
          <?php echo "$mtxversion<br/>" ?>
        </td>
        <th colspan="2">
          <br/>
        </th>
      </tr>
      <tr>
        <th>Loader Description:</th>
        <td colspan="3">
          <?php echo "$description<br/>" ?>
        </td>
      </tr>
      <tr>
        <th style="text-align: center" colspan="4">LoaderInfo Output</th>
      </tr>
      <tr>
        <th>Vendor ID:</th>
        <td>
          <?php echo "$vendorid<br/>" ?>
        </td>
        <th>Product ID:</th>
        <td>
          <?php echo "$productid<br/>" ?>
        </td>
      </tr>
      <tr>
        <th>Revision:</th>
        <td>
          <?php echo "$revision<br/>" ?>
        </td>
        <th>SerialNumber:</th>
        <td>
          <?php echo "$serialnum<br/>" ?>
        </td>
      </tr>
      <tr>
        <th>Barcode Reader:</th>
        <td>
          <?php echo $barcodes == 1 ? "Yes" : "No" ?>
        </td>
        <th>Element Address Assignment Page:</th>
        <td>
          <?php echo $eaap == 1 ? "Yes" : "No" ?>
        </td>
      </tr>
      <tr>
        <th>Transfer Geometry Descriptor Page:</th>
        <td>
          <?php echo $tgdp == 1 ? "Yes" : "No" ?>
        </td>
        <th>Can Transfer:</th>
        <td>
          <?php echo $canxfer == 1 ? "Yes" : "No" ?>
        </td>
      </tr>
      <tr>
        <th>Number of Medium Transport Elements:</th>
        <td>
          <?php echo "$transports" ?>
        </td>
        <th>Number of Storage Elements:</th>
        <td>
          <?php echo "$slots" ?>
        </td>
      </tr>
      <tr>
        <th>Number of Import/Export Elements:</th>
        <td>
          <?php echo "$imports" ?>
        </td>
        <th>Number of Data Transfer Elements:</th>
        <td>
          <?php echo "$transfers" ?>
        </td>
      </tr>
      <tr>
        <th style="text-align: center" colspan="4">Comments</th>
      </tr>
      <tr>
        <td colspan="4">
          <?php echo "$comments<br/>" ?>
        </td>
      </tr>
      <tr>
        <td colspan="4">
          <table border="1" width="100%">
            <tr>
              <th width="50%">Submitted By:</th>
              <td width="50%">
                <?php echo "$name" ?>
              </td>
            </tr>
          </table>
        </td>
      </tr>
<?php
  } else {
?>
      <tr>
        <td align="center" colspan="5">RECORD NOT FOUND IN DATABASE</td>
      </tr>
<?php
  }
?>
    </table>
  </body>
</html>
