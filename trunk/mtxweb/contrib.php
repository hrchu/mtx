<?php
  include('dbms_inc.php');

  $link = mysql_connect($mysql_host,$mysql_user,$mysql_password)
       or die("Could not connect");
  mysql_select_db($mysql_dbms) or die("Could not select database");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-type" content="text/html;charset=UTF-8" />
    <meta http-equiv="Pragma" content="no-cache" />
    <meta name="Content-script-type" content="text/javascript" />

    <title>MTX Compatibility List - Contribute</title>
    
    <style type="text/css">
      <!--
        TH { background-color: aqua; text-align: right; }
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
          
          if (field.value == null || field.value == "")
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
        
        function validateLoaderInfoText(e)
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

          return keyCode != 39;
        }

        function validateSubmit()
        {
          var e_osversion = document.getElementById("osversion");
          var e_description = document.getElementById("description");
          var e_vendorid = document.getElementById("vendorid");
          var e_productid = document.getElementById("productid");
          var e_revision = document.getElementById("revision");

          if (e_osversion.value == "")
          {
            window.alert("You must enter the version of your operating system.  Please use the output of the command 'uname -svrp' on UNIX systems.");
            e_osversion.focus();
            return false;
          }

          if (e_description.value == "")
          {
            window.alert("You must enter a description of your loader device.");
            e_description.focus();
            return false;
          }

          if (e_vendorid.value == "" || e_productid.value == "" || e_revision.value == "")
          {
            window.alert("You must enter the Vendor ID, Product ID and Revision listed by loaderinfo.");

            if (e_vendorid.value == "")
            {
              e_vendorid.focus();
            }
            else if (e_productid.value == "")
            {
              e_productid.focus();
            }
            else if (e_revision.value == "")
            {
              e_revision.focus();
            }

            return false;
          }

          return true;
        }
      -->
    </script>
  </head>
  <body>
    <table width="100%" cellspacing="0" cellpadding="10">
      <tr valign="middle">
        <th></th>
        <th style="text-align: center">
          <h1>MTX Compatibility List<br />Contribute</h1>
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
            <span style="margin-left: 1em; font-weight:bold">Submit</span>
            <br />
          </p>
          <p>
            <a href="faq.html">FAQ</a>
          </p>
        </th>
        <td rowspan="2">
          This application is currently in beta test and may have bugs. 
          Please only submit entries that you have verified as being compatible with 
          'mtx'.  <b>Please note that your EMAIL address will *NOT* be published, and 
          will be used only in the event that I have questions about your entry.</b>
          <p />
          You will need the following information:
          <ol>
            <li>
              Your operating system type and version number (e.g. "uname -svrp"
              on FreeBSD, Linux, and Solaris)
            </li>
            <li>
              Your MTX version ( mtx --version )
            </li>
            <li>
              The result of 'loaderinfo' on your loader
            </li>
          </ol> 
          Please note that the 'barcode' output from 'loaderinfo' is not accurate for
          most loaders. Please report whether barcodes actually show up when you do
          'mtx status'. 
          <p />
          <form action="verify.php" method="post"  onsubmit="return validateSubmit()">
            <p>
              <input type="hidden" name="l_enabled" value="1" />
              <input type="hidden" name="l_worked" value="1" />
              <table border="1">
                <tr>
                  <th style="text-align: center" colspan="4">OS and General Info</th>
                </tr>
                <tr>
                  <!-- do a pulldown for operating system name:  -->
                  <th>Operating System</th>
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
                  <th>MTX Version</th>
                  <td>
                    <select name="l_mtxversion">
  <?php
    $query_str = "select version from versions order by `key`";
    $result = mysql_query($query_str,$link) or die("</th></tr></table>Invalid query string '$query_str'");
    $num_rows = mysql_num_rows($result);

    for ($row_no = 1; $row_no <= $num_rows; $row_no++) {
      $row = mysql_fetch_assoc($result);
      echo '<option value="',$row['version'],$row_no == $num_rows ? '" selected="selected">' : '">',$row['version'],'</option>';
    }
  ?>
                    </select>
                  </td>
                </tr>
                <tr>
                  <th>OS Version</th>
                  <td colspan="3">
                    <input id="osversion" name="l_osversion" type="text" size="80" maxlength="100"/>
                  </td>
                </tr>
                <tr>
                  <th>Loader Description</th>
                  <td colspan="3">
                    <input id="description" name="l_description" type="text" size="80" maxlength="100"/>
                  </td>
                </tr>
                <tr>
                  <th style="text-align: center" colspan="4">LoaderInfo Output</th>
                </tr>
                <tr>
                  <th>Vendor ID</th>
                  <td>
                    <input id="vendorid" name="l_vendorid" type="text" size="8" maxlength="8" onkeypress="return validateLoaderInfoText(event)"/>
                  </td>
                  <th>Product ID</th>
                  <td>
                    <input id="productid" name="l_productid" type="text" size="16" maxlength="16" onkeypress="return validateLoaderInfoText(event)"/>
                  </td>
                </tr>
                <tr>
                  <th>Revision</th>
                  <td>
                    <input id="revision" name="l_revision" type="text" size="4" maxlength="4" onkeypress="return validateLoaderInfoText(event)"/>
                  </td>
                  <th>Serial Number</th>
                  <td>
                    <input name="l_serialnum" type="text" size="25" maxlength="25"/>
                  </td>
                </tr>
                <tr>
                  <th>Barcode Reader</th>
                  <td>
                    <select name="l_barcodes">
                      <option value="0">No</option>
                      <option value="1">Yes</option>
                    </select>
                  </td>
                  <th>Element Address Assignment Page (EAAP)</th>
                  <td>
                    <select name="l_eaap">
                      <option value="0">No</option>
                      <option value="1">Yes</option>
                    </select>
                  </td>
                </tr>
                <tr>
                  <th>Transport Geometry Descriptor Page</th>
                  <td>
                    <select name="l_tgdp">
                      <option value="0">No</option>
                      <option value="1">Yes</option>
                    </select>
                  </td>
                  <th>Can Transfer</th>
                  <td>
                    <select name="l_canxfer">
                      <option value="0">No</option>
                      <option value="1">Yes</option>
                    </select>
                  </td>
                </tr>
                <tr>
                  <th>Number of Medium Transport Elements</th>
                  <td>
                    <input name="l_transports" type="text" value="0" size="5" maxlength="5" onkeypress="return validateNumeric(event)" onchange="return validateShort(this)"/>
                  </td>
                  <th>Number of Storage Elements</th>
                  <td>
                    <input name="l_slots" type="text" value="0" size="5" maxlength="5" onkeypress="return validateNumeric(event)" onchange="return validateShort(this)"/>
                  </td>
                </tr>
                <tr>
                  <th>Number of Import/Export Elements</th>
                  <td>
                    <input name="l_imports" type="text" value="0" size="5" maxlength="5" onkeypress="return validateNumeric(event)" onchange="return validateShort(this)"/>
                  </td>
                  <th>Number of Data Transfer Elements</th>
                  <td>
                    <input name="l_transfers" type="text" value="0" size="5" maxlength="5" onkeypress="return validateNumeric(event)" onchange="return validateShort(this)"/>
                  </td>
                </tr>
                <tr>
                  <th style="text-align: center" colspan="4">Comments</th>
                </tr>
                <tr>
                  <td colspan="4" align="center">
                    <textarea name="l_comments" cols="70" rows="4"></textarea>
                  </td>
                </tr>
                <tr>
                  <th style="text-align: center" colspan="4">Personal Data</th>
                </tr>
                <tr>
                  <th>Your Name</th>
                  <td>
                    <input name="l_name" type="text" size="25" maxlength="80"/>
                  </td>
                  <th>Your EMAIL Address</th>
                  <td>
                    <input name="l_email" type="text" size="25" maxlength="80"/>
                  </td>
                </tr>
                <tr>
                  <td align="right" colspan="2">
                    <input type="submit" name="Save" value="Save"/>
                  </td>
                  <td colspan="2">
                    <input type="button" name="Cancel" value="Cancel" onclick="history.go(-1)"/>
                  </td>
                </tr>
              </table>
            </p>
          </form>
          <hr />
          <table style="font-size:small" width="100%">
            <tr>
              <td style="text-align:left; width:33%">
                Maintained by <a href="mailto:robertnelson@users.sourceforge.net">Robert Nelson</a>
              </td>
              <td style="text-align:center; width:34%">
                <?php
                  $ChangedDate = preg_replace('/.*: (.+) \(.*/', '\1', '$LastChangedDate$');
                  echo "Date changed: $ChangedDate";
                ?>
              </td>
              <td style="text-align:right; width:33%">
                <?php
                  $ChangedBy = preg_replace('/.*: (.+) \$/', '\1', '$LastChangedBy$');
                  echo "Changed by: $ChangedBy";
                ?>
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
              <img src="valid-xhtml10.png" alt="Valid XHTML 1.0 Strict" 
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
