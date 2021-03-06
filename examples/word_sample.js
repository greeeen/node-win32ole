var win32ole = require('win32ole');
win32ole.print('word_sample\n');
console.log(win32ole.version());
var path = require('path');
var cwd = path.join(win32ole.MODULEDIRNAME, '..');

var fs = require('fs');
var tmpdir = path.join(cwd, 'test/tmp');
if(!fs.existsSync(tmpdir)) fs.mkdirSync(tmpdir);
var outfile = path.join(tmpdir, 'word_sample.doc');

var word_sample = function(filename){
  var wd = win32ole.client.Dispatch('Word.Application');
  wd.set('Visible', true);
  var doc = wd.get('Documents').call('Add');
  var para = doc.get('Content').get('Paragraphs').call('Add');
  para.get('Range').set('Text', 'stringUTF8');
  try{
    console.log('saving to: "' + filename + '" ...');
    var result = doc.call('SaveAs', [filename]);
    console.log(result); // .toBoolean() is not VT_BOOL
  }catch(e){
    console.log('(exception cached)\n' + e);
  }
  wd.get('Documents').call('Close');
  wd.call('Quit');
};

try{
  word_sample(outfile);
}catch(e){
  console.log('*** exception cached ***\n' + e);
}
