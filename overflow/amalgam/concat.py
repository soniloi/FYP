import os

sep = '/*****************************************************************************/\n'

# Output formatted header to file
def output_header(fileout, label):
  fileout.write('\n' + sep + '/** ' + label + ' **/\n' + sep + '\n')

# Concatenate various constituents into a source file
def concat_source(fileoutpath, headerpath, prototypespath, macrodir, funcdir, indarr):
  fileout = open(fileoutpath, 'w+')

  # Generate file; output 'header'
  output_header(fileout, 'Global Declarations')
  with open(headerpath, 'r') as headerfile:
    for line in headerfile:
      fileout.write(line)

  # Concatenate forward declarations
  output_header(fileout, 'Forward Declarations')
  with open(prototypespath, 'r') as prototypesfile:
    for line in prototypesfile:
      fileout.write(line)

  # Concatenate macros in original order
  output_header(fileout, 'Macros')
  macropaths = os.listdir(macrodir)
  macropaths.sort()
  for macrofilepath in macropaths:
    with open(macrodir + '/' + macrofilepath, 'r') as macrofile:
      for line in macrofile:
        fileout.write(line)
      fileout.write('\n\n')

  # Concatenate functions in specified order
  output_header(fileout, 'Functions')
  funcpaths = os.listdir(funcdir)
  funcpaths.sort()
  for index in indarr:
    #print str(index) + '\t',
    #if index%20 == 0:
    #  print
    with open(funcdir + '/' + funcpaths[index], 'r') as funcfile:
      for line in funcfile:
        fileout.write(line)
    fileout.write('\n\n')

  fileout.close()

