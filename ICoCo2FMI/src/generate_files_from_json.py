from __future__ import print_function

import json


import sys

with open(sys.argv[1]) as json_file:
    data = json.load(json_file)
    for p in data['vars']:
        print('Name: ' + p['varname'])
        print('type: ' + p['type'])
        print('nout: ' + p['inout'])
        print('')

vartype = {}
Types = ["Integer", "Real", "String"]
for t in Types:
    vartype[t] = []
for var in data['vars']:
    tt = var['type']
    try:
        vartype[tt].append(var)
    except KeyError:
        raise Exception(tt+" not implemented")
f = open("vars_def.h", "w")
for t in Types:
    f.write(" comp->names_%s = new std::vector<std::string>(%d);\n"
            % (t, len(vartype[t])))
    f.write(" comp->input_%s = new std::vector<bool>(%d);\n"
            % (t, len(vartype[t])))
    for i, var in enumerate(vartype[t]):
        f.write(' comp->names_%s->operator[](%d)=\"' % (t, i) +
                var['varname'] + '\";\n')
        f.write(' comp->input_%s->operator[](%d)=' % (t, i))
        if var['inout'] == 'input':
            f.write('true;\n')
        else:
            f.write('false;\n')

MODEL_NAME = sys.argv[2]
MODEL_GUID = sys.argv[3]


template_xml = """<?xml version="1.0" encoding="ISO-8859-1"?>
<fmiModelDescription
  fmiVersion="2.0"
  modelName="__MODEL_NAME__"
  guid="__MODEL_GUID__"
  numberOfEventIndicators="0">

<CoSimulation
  modelIdentifier="__MODEL_NAME__"
  canHandleVariableCommunicationStepSize="true">
</CoSimulation>

<LogCategories>
  <Category name="logAll"/>
  <Category name="logError"/>
  <Category name="logFmiCall"/>
  <Category name="logEvent"/>
</LogCategories>

<ModelVariables>
  __VARS__
</ModelVariables>

<ModelStructure>
  <Outputs>
  __VARS2__
  </Outputs>
</ModelStructure>

</fmiModelDescription>
"""
template_var = {}
template_var['output'] = """<ScalarVariable name="__name__"
       description="__description__"
       valueReference="__valueReference__"
       causality="__causality__"
       variability="discrete"
       initial='exact'>
       <__type__ start="0"/>
  </ScalarVariable>
"""
template_var['input'] = """<ScalarVariable name="__name__"
    description="__description__"
    valueReference="__valueReference__"
    causality="__causality__"
    variability="discrete" >
    <__type__ start="0"/>
  </ScalarVariable>
"""
VARS = ""
listout = []
n1 = 1
for t in Types:
    varst = vartype[t]
    for i, var in enumerate(varst):
        toto = template_var[var['inout']]
        toto = toto.replace('__name__', var['varname'])
        toto = toto.replace('__valueReference__', str(i))
        toto = toto.replace('__type__', var['type'])
        toto = toto.replace('__causality__', var['inout'])
        if var['inout'] == 'output':
            listout.append(n1)
        toto = toto.replace('__description__', var['description'])
        VARS += toto
        n1 += 1
VARS2 = ""
for i in listout:
    VARS2 += '<Unknown index="'+str(i)+'"/>\n'

with open('modelDescription_cs.xml', 'w') as outfile:
    outfile.write(template_xml.replace('__VARS__', VARS)
                  .replace('__VARS2__', VARS2)
                  .replace('__MODEL_NAME__', MODEL_NAME)
                  .replace('__MODEL_GUID__', MODEL_GUID))
