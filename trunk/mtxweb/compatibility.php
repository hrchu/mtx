<?php
  /* set these to what you need for your installation */
  include('../dbfiles/dbms.data');

  $sorttype = $_GET['sorttype'];

  $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
    or die("Could not connect");
  mysql_select_db($mysql_dbms) or die("Could not select database");

  if ("$sorttype" == "" || $sorttype < 1 || $sorttype > 4)
  {
    $sorttype = 1;
  }

  $join_on = "";
  
  switch ($sorttype)
  {
  case 1: // OS
    $order_by = "osname,osversion,vendorid,description,mtxversion";
    break;
  case 2: // Vendor
    $order_by = "vendorid,description,osname,osversion,mtxversion";
    break;
  case 3: // Description
    $order_by = "description,vendorid,osname,osversion,mtxversion";
    break;
  case 4: // MTX Version
    $order_by = "`key`,osname,osversion,vendorid";
    $join_on = "inner join versions on mtxversion = version";
    break;
  }

  $query_str = "select id,osname,osversion,vendorid,description,mtxversion from loaders $join_on where enabled = 1 order by $order_by";
  $result = mysql_query($query_str,$link) or die("Invalid query '$query_str'");
  $num_rows = mysql_num_rows($result);

?>
<!DOCTYPE html PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>
    <meta http-equiv="Pragma" content="no-cache"/>
    <title>MTX Compatibility List - Summary</title>

    <style type="text/css">
      <!--
        TH { background-color: cyan;  }
        TR { background-color: white; }
        H2 { text-align: center; }
        BODY { background-color: white; }
        A:link { color: blue; text-decoration: underline ; }
        A:active { text-decoration: underline ; }
        A:hover { text-decoration: underline ; }
        A:visited { color: blue; text-decoration: underline ; }
      -->
    </style>
  </head>

  <body>
    <a href="http://sourceforge.net">
      <img src="http://sflogo.sourceforge.net/sflogo.php?group_id=4626&amp;type=7" width="210" height="62" border="0" style="position:absolute;left:0.125in;" alt="SourceForge.net Logo"/>
    </a>
    <h2>MTX Compatibility List<br/>Summary</h2>
    <br clear="left" />
    <br/>
This application is currently in beta test, and may be buggy. The database
that it feeds off of is currently very incomplete. Please 
<a href="contrib.php">contribute</a> new entries so that others can
benefit.
    <p/>
You can change the sort order by clicking on the underlined column heading.
    <p/>
In order to display the detailed information click on the desired line.
    <p/>
    <table border="1" width="100%">
      <tr>
<?php 
  if ($sorttype == 1) {
?>
        <th>OS</th>
        <th>OS Version</th>
        <th>
          <a href="compatibility.php?sorttype=2">Vendor</a>
        </th>
        <th>
          <a href="compatibility.php?sorttype=3">Description</a>
        </th>
        <th>
          <a href="compatibility.php?sorttype=4">MTX Version</a>
        </th>
<?php 
  } else if ($sorttype == 2) {
?>
        <th>Vendor</th>
        <th>
          <a href="compatibility.php?sorttype=3">Description</a>
        </th>
        <th>
          <a href="compatibility.php?sorttype=1">OS</a>
        </th>
        <th>OS Version</th>
        <th>
          <a href="compatibility.php?sorttype=4">MTX Version</a>
        </th>
<?php 
  } else if ($sorttype == 3) {
?>
        <th>Description</th>
        <th>
          <a href="compatibility.php?sorttype=2">Vendor</a>
        </th>
        <th>
          <a href="compatibility.php?sorttype=1">OS</a>
        </th>
        <th>OS Version</th>
        <th>
          <a href="compatibility.php?sorttype=4">MTX Version</a>
        </th>
<?php 
  } else {
?>
        <th>MTX Version</th>
        <th>
          <a href="compatibility.php?sorttype=1">OS</a>
        </th>
        <th>OS Version</th>
        <th>
          <a href="compatibility.php?sorttype=2">Vendor</a>
        </th>
        <th>
          <a href="compatibility.php?sorttype=3">Description</a>
        </th>
<?php
  }
?>
      </tr>
<?php
  if ($num_rows==0) {
?>
      <tr>
        <th style="background-color: white">NO RECORDS IN DATABASE</th>
      </tr>
<?php 
  } else { 
    while ($row = mysql_fetch_assoc($result)) {
      extract($row);

      echo "<tr onClick=\"location.href='detail.php?record=$id'\">";

      if ($sorttype == 1) {
?>
        <td><?php echo "$osname<br/>"; ?></td>
        <td><?php echo "$osversion<br/>"; ?></td>
        <td><?php echo "$vendorid<br/>"; ?></td>
        <td><?php echo "$description<br/>"; ?></td>
        <td><?php echo "$mtxversion<br/>"; ?></td>
<?php 
      } else if ($sorttype == 2) {
?>
        <td><?php echo "$vendorid<br/>"; ?></td>
        <td><?php echo "$description<br/>"; ?></td>
        <td><?php echo "$osname<br/>"; ?></td>
        <td><?php echo "$osversion<br/>"; ?></td>
        <td><?php echo "$mtxversion<br/>"; ?></td>
<?php 
      } else if ($sorttype == 3) {
?>
        <td><?php echo "$description<br/>"; ?></td>
        <td><?php echo "$vendorid<br/>"; ?></td>
        <td><?php echo "$osname<br/>"; ?></td>
        <td><?php echo "$osversion<br/>"; ?></td>
        <td><?php echo "$mtxversion<br/>"; ?></td>
<?php 
      } else {
?>
        <td><?php echo "$mtxversion<br/>"; ?></td>
        <td><?php echo "$osname<br/>"; ?></td>
        <td><?php echo "$osversion<br/>"; ?></td>
        <td><?php echo "$vendorid<br/>"; ?></td>
        <td><?php echo "$description<br/>"; ?></td>
<?php
      }
      echo "</tr>";
    }
  }
?>
    </table>
  </body>
</html>
