# NAME

node-win32ole - Asynchronous, non-blocking win32ole bindings for [node.js](https://github.com/joyent/node) .


# USAGE

Install with `npm install win32ole`.

It works (expected) as...

``` js
var win32ole = require('win32ole');
var xl = win32ole.client.Dispatch('Excel.Application');
xl.Visible = true;
var book = xl.Workbooks.Add();
var sheet = book.Worksheets(1);
sheet.Name = 'sheetnameA utf8';
sheet.Cells(1, 2).Value = 'test utf8';
var rg = sheet.Range(sheet.Cells(2, 2), sheet.Cells(4, 4));
rg.RowHeight = 5.18;
rg.ColumnWidth = 0.58;
rg.Interior.ColorIndex = 6; // Yellow
book.SaveAs('testfileutf8.xls');
xl.ScreenUpdating = true;
xl.Workbooks.Close();
xl.Quit();
```

But now it implements as... (comming soon)

``` js
var win32ole = require('win32ole');
var st = new win32ole.Statement;
var xl = st.Dispatch('Excel.Application');
xl.set('Visible', true);
var book = xl.get('Workbooks').call('Add', []);
var sheet = book.call('Worksheets', [1]);
sheet.set('Name', 'sheetnameA utf8');
sheet.call('Cells', [1, 2]).set('Value', 'test utf8');
var rg = sheet.call('Range',
  [sheet.call('Cells', [2, 2]), sheet.call('Cells', [4, 4])]);
rg.set('RowHeight', 5.18);
rg.set('ColumnWidth', 0.58);
rg.get('Interior').set('ColorIndex', 6); // Yellow
book.call('SaveAs', ['testfileutf8.xls']);
xl.set('ScreenUpdating', true);
xl.get('Workbooks').call('Close', []);
xl.call('Quit', []);
st.Finalize(); // must be called now
```


# FEATURES

* So much implements.
* Implement accessors getter, setter and caller.
* npm


# API

See the [API documentation](https://github.com/idobatter/node-win32ole/wiki) in the wiki.


# BUILDING

This project uses VC++ 2008 Express (or later) and Python 2.6 (or later) .
(When using Python 2.5, it needs [multiprocessing 2.5 back port](http://pypi.python.org/pypi/multiprocessing/) .)

Bulding also requires node-gyp to be installed. You can do this with npm:

    npm install -g node-gyp

To obtain and build the bindings:

    git clone git://github.com/idobatter/node-win32ole.git
    cd node-win32ole
    node-gyp configure
    node-gyp build

You can also use [`npm`](https://github.com/isaacs/npm) to download and install them:

    npm install win32ole


# TESTS

[mocha](https://github.com/visionmedia/mocha) is required to run unit tests.

    npm install -g mocha
    nmake /a test


# CONTRIBUTORS

* [idobatter](https://github.com/idobatter)


# ACKNOWLEDGEMENTS

Inspired [Win32OLE](http://www.ruby-doc.org/stdlib/libdoc/win32ole/rdoc/)


# LICENSE

`node-win32ole` is [BSD licensed](https://github.com/idobatter/node-win32ole/raw/master/LICENSE).