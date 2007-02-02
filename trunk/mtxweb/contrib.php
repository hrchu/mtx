<?php
  include('../dbfiles/dbms.data');

  $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
       or die("Could not connect");
  mysql_select_db($mysql_dbms) or die("Could not select database");
?>
<!DOCTYPE html PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>
    <meta name="Content-script-type" content="text/javascript"/>
    <title>Contribute to MTX compatibility list</title>
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
    <script type="text/javascript">
      <!--
        function validateShort(field)
        {
          var value;
          
          if (field.value == null)
          {
            field.value = "0";
            return true;
          }
          
          value = parseInt(field.value);

          if (isNaN(value))
          {
            field.value = "0";
            return true;
          }

          if (value > 65535)
          {
            window.alert("Value must be between 0 and 65535");
            return false;
          }
          return true;
        }

        function validateNumeric(e)
        {
          var keyCode;

          if (e.which)
          {
             keyCode = e.which;
          }
          else if (e.keyCode)
          {
             keyCode = e.keyCode;
          }
          else 
          {
             return true;
          }

          return keyCode < 32 || keyCode == null || (keyCode >= 48 && keyCode <= 57);
        }
      -->
    </script>
  </head>
  <body bgcolor="white">
    <a href="http://sourceforge.net">
      <img src="http://sflogo.sourceforge.net/sflogo.php?group_id=4626&amp;type=7" width="210" height="62" border="0" style="position:absolute;left:0.125in;" alt="SourceForge.net Logo"/>
    </a>
    <h2>Contribute to MTX Compatibility List</h2>
    <br clear="left" />
    <br/>
WARNING: This application is currently in beta test, and may be buggy.
Please only use it to submit entries that you have verified as being
compatible with 'mtx'.  <b>Please note that your EMAIL address will *NOT*
be published, and will be used only in the event that I have questions about
your entry. </b>
    <p/>
You will need the following information:
    <ol>
      <li>
        Your operating system type and version number (e.g. "uname -svrp"
        on FreeBSD, Linux, and Solaris)</li>
      <li> Your MTX version ( mtx --version )</li>
      <li> The result of 'loaderinfo' on your loader</li>
    </ol> 

Please note that the 'barcode' output from 'loaderinfo' is not accurate for
most loaders. Please report whether barcodes actually show up when you do
'mtx status'. 

    <p/>
    <form action="verify.php" method="POST">
      <input type="hidden" name="l_enabled" value="1"/>
      <input type="hidden" name="l_worked" value="1"/>
      <!-- now for the table: -->
      <table border="1">
        <tr>
          <th style="text-align: center" colspan="4">OS and General Info</th>
        </tr>
        <tr>
          <!-- do a pulldown for operating system name:  -->
          <th>Operating System:</th>
          <td>
            <select name="l_osname">
<?php
  $query_str="select osname from hosts";
  $result=mysql_query($query_str,$link) or die("</th></tr></table>Invalid query string '$query_str'");

  while ($row=mysql_fetch_assoc($result)) {
    echo '<option value="',$row['osname'],'">',$row['osname'],'</option>';
  }
?>
            </select>
          </td>
          <th>OS Version:</th>
          <td>
            <input name="l_osversion" type="text" size="25" maxlength="100"/>
          </td>
        </tr>
        <tr>
          <th>MTX Version:</th>
          <td>
            <select name="l_mtxversion">
<?php
  $query_str = "select version from versions";
  $result = mysql_query($query_str,$link) or die("</th></tr></table>Invalid query string '$query_str'");
  $num_rows = mysql_num_rows($result);

  for ($row_no = 1; $row_no <= $num_rows; $row_no++) {
    $row = mysql_fetch_assoc($result);
    echo '<option value="',$row['version'],$row_no == $num_rows ? '" SELECTED>' : '">',$row['version'],'</option>';
  }
?>
            </select>
          </td>
          <th colspan="2">
            <br/>
          </th>
        </tr>
        <tr>
          <th>Loader Description:</th>
          <td colspan="3">
            <input name="l_description" type="text" size="80" maxlength="100"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:center" colspan="4">LoaderInfo Output</th>
        </tr>
        <tr>
          <th>Vendor ID:</th>
          <td>
            <input name="l_vendorid" type="text" size="8" maxlength="8"/>
          </td>
          <th>Product ID:</th>
          <td>
            <input name="l_productid" type="text" size="16" maxlength="16"/>
          </td>
        </tr>
        <tr>
          <th>Revision:</th>
          <td>
            <input name="l_revision" type="text" size="4" maxlength="4"/>
          </td>
          <th>Serial Number:</th>
          <td>
            <input name="l_serialnum" type="text" size="25" maxlength="25"/>
          </td>
        </tr>
        <tr>
          <th>Barcode Reader:</th>
          <td>
            <select name="l_barcodes">
              <option value="0">No</option>
              <option value="1">Yes</option>
            </select>
          </td>
          <th>Element Address Assignment Page (EAAP):</th>
          <td>
            <select name="l_eaap">
              <option value="0">No</option>
              <option value="1">Yes</option>
            </select>
          </td>
        </tr>
        <tr>
          <th>Transfer Geometry Descriptor Page:</th>
          <td>
            <select name="l_tgdp">
              <option value="0">No</option>
              <option value="1">Yes</option>
            </select>
          </td>
          <th>Can Transfer:</th>
          <td>
            <select name="l_canxfer">
              <option value="0">No</option>
              <option value="1">Yes</option>
            </select>
          </td>
        </tr>
        <tr>
          <th>Number of Medium Transport Elements:</th>
          <td>
            <input name="l_transports" type="text" size="5" maxlength="5" onKeyPress="return validateNumeric(event)" onChange="return validateShort(this)"/>
          </td>
          <th>Number of Storage Elements:</th>
          <td>
            <input name="l_slots" type="text" size="5" maxlength="5" onKeyPress="return validateNumeric(event)" onChange="return validateShort(this)"/>
          </td>
        </tr>
        <tr>
          <th>Number of Import/Export Elements:</th>
          <td>
            <input name="l_imports" type="text" size="5" maxlength="5" onKeyPress="return validateNumeric(event)" onChange="return validateShort(this)"/>
          </td>
          <th>Number of Data Transfer Elements:</th>
          <td>
            <input name="l_transfers" type="text" size="5" maxlength="5" onKeyPress="return validateNumeric(event)" onChange="return validateShort(this)"/>
          </td>
        </tr>
        <tr>
          <th style="text-align: center" colspan="4">Comments</th>
        </tr>
        <tr>
          <td colspan="4" align="center">
            <textarea name="l_comments" cols="70" rows="4" wrap="virtual"></textarea>
          </td>
        </tr>
        <tr>
          <th style="text-align: center" colspan="4">Personal Data</th>
        </tr>
        <tr>
          <th>Your Name:</th>
          <td>
            <input name="l_name" type="text" size="25" maxlength="80"/>
          </td>
          <th>Your EMAIL Address:</th>
          <td>
            <input name="l_email" type="text" size="25" maxlength="80"/>
          </td>
        </tr>
        <tr>
          <td align="right" colspan="2">
            <input type="submit" name="Save" value="Save"/>
          </td>
          <td colspan="2">
            <input type="button" name="Cancel" value="Cancel" onClick="history.go(-1)"/>
          </td>
        </tr>
      </table>
    </form>
  </body>
</html>
