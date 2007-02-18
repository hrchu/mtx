<?php
  /* set these to what you need for your installation */
  include('dbms_data.php');

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
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-type" content="text/html;charset=UTF-8" />
    <meta http-equiv="Pragma" content="no-cache"/>

    <title>MTX Compatibility List - Summary</title>

    <style type="text/css">
      <!--
        TH { background-color: aqua;  }
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
    <table width="100%" cellspacing="0" cellpadding="10">
      <tr valign="middle">
        <th></th>
        <th>
          <h1>MTX Compatibility List<br/>Summary</h1>
        </th>
      </tr>
      <tr>
        <th style="vertical-align:top; text-align:left">
          <h2>Links:</h2>
          <p>
            <a href="http://sourceforge.net/projects/mtx">SourceForge Project</a>
          </p>
          <p>
            <a href="http://mtx.opensource-sw.net">Home Page</a>
          </p>
          <p>
            <a href="http://sourceforge.net/project/showfiles.php?group_id=4626">Downloads</a>
          </p>
          <p style="font-weight:bold; text-decoration:underline">Compatibility</p>
          <p>
            <a href="compatibility.php" style="margin-left: 1em">View</a>
            <br />
            <a href="contrib.php" style="margin-left: 1em">Submit</a>
            <br />
          </p>
          <p>
            <a href="faq.html">FAQ</a>
          </p>
        </th>
        <td rowspan="2">
          This application is currently in beta test and may have bugs. The database
          is currently very incomplete. Please <a href="contrib.php">submit</a> new 
          entries so that others can benefit.
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

      echo "<tr onclick=\"location.href='detail.php?record=$id'\">";

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
          <br />
          <hr />
          <table style="font-size:small; " width="100%">
            <tr>
              <td style="text-align:left" width="33%">
                Maintained by <a href="mailto:robertnelson@users.sourceforge.net">Robert Nelson</a>
              </td>
              <td style="text-align:center" width="34%">
                $LastChangedDate$
              </td>
              <td style="text-align:right" width="33%">
                $LastChangedBy$
              </td>
            </tr>
          </table>
        </td>
      </tr>
      <tr>
        <th style="text-align:center; vertical-align:middle">
          <p>
            <a href="http://www.dreamhost.com/r.cgi?277748">
              <img src="dh-100x75.gif" alt="DreamHost.com Logo"
                   height="75" width="100" style="border:0" />
            </a>
          </p>
          <p>
            <a href="http://validator.w3.org/check?uri=referer" >
              <img src="valid-xhtml10.png" alt="Valid XHTML 1.0 Transitional" 
                   height="31" width="88" style="border:0" />
            </a>
          </p>
          <p>
            <a href="http://sourceforge.net/projects/mtx">
              <img src="http://sflogo.sourceforge.net/sflogo.php?group_id=4626&amp;type=1"
                   height="31" width="81" style="border:0" alt="SourceForge.net Logo" />
            </a>
          </p>
        </th>
      </tr>
    </table>
  </body>
</html>
