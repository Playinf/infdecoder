#!/usr/bin/python3
# out2sgm.py
# converting plain format to sgm format
#
# author: hjm
#

import optparse,os,sys
#reload(sys)
#sys.setdefaultencoding('utf-8')

parser = optparse.OptionParser(usage = './%prog input.sgm tran.out [Options]',version = '%prog 0.1 for python 3')

parser.add_option('-o','--out-sgm-file',action = 'store',dest = 'out_sgm',type='string',default='out.sgm',help='assign the OUTPUT file path,default the same folder path with input.sgm',metavar = 'OUTPUT')
parser.add_option('-c','--encoding',action='store',dest = 'encoding',type='string',default='utf-8',help ='assign the encoding of input.sgm,tran.out,out.sgm',metavar='encoding')

(opts,args) = parser.parse_args()

in_sgm_path = args[0]
translation_file_path = args[1]
out_sgm_path = opts.out_sgm 
encoding = opts.encoding

in_sgm_folderpath = ''
if out_sgm_path == 'out.sgm':
    in_sgm_folderpath = os.path.dirname(in_sgm_path)
    out_sgm_path = in_sgm_folderpath + out_sgm_path

in_sgm = open(in_sgm_path,'rb')
out_sgm = open(out_sgm_path,'wb')
translation_file = open(translation_file_path,'rb')
in_line = ''
tr_line = ''

while True:
    in_line = in_sgm.readline()
    if len(in_line) == 0:
        break
    if in_line.startswith('<srcset'.encode(encoding)):
        out_sgm.write(in_line.replace('<srcset'.encode(encoding),'<tstset'.encode(encoding)))
    elif in_line.startswith('</srcset'.encode(encoding)):
        out_sgm.write(in_line.replace('</srcset'.encode(encoding),'</tstset'.encode(encoding)))
    elif in_line.startswith('<DOC'.encode(encoding)) or in_line.startswith('<doc'.encode(encoding)):
        out_sgm.write(in_line.replace('>'.encode(encoding),' sysid="hpb">'.encode(encoding)))
    elif in_line.startswith('<seg id'.encode(encoding)):
        tr_line = translation_file.readline()
        tr_vec = tr_line.split('|||'.encode(encoding))
        out_line = in_line[0:in_line.find('>'.encode(encoding))+1] + tr_vec[1] + '</seg>\n'.encode(encoding)
        out_sgm.write(out_line)
    else:
        out_sgm.write(in_line)
in_sgm.close()
out_sgm.flush()
out_sgm.close()
translation_file.close()







