import os
import sys
import codecs
import chardet

def convert(file, inType):
    try:
        print(inType + " to utf8 : " + file, flush = True)
        f = codecs.open(file, 'r', inType)
        content = f.read()
        codecs.open(file, 'w', "UTF-8").write(content)
    except IOError as err:
        print("IOERR:" + err)

for root, dirs, files in os.walk(sys.argv[1]):
    for fn in files:
        path = root + "/" + fn
        with open(path, 'rb') as f:
            data = f.read()
            ct = chardet.detect(data)['encoding']
            convert(path, ct)
print("转换完成! ╰(*°▽°*)╯")