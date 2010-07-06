#!/usr/bin/env python
# -*- coding: utf-8 -*-
from distutils.cmd import Command
import doctest
from glob import glob
import os
import sys
try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

class build_doc(Command):
    description = 'Builds the OpenRAVE python documentation'
    user_options = [
        ('force', None,
         "force regeneration even if no reStructuredText files have changed"),
        ('without-apidocs', None,
         "whether to skip the generation of API documentaton"),
        ('outdir=',None,
         "output dir of the document")
    ]
    boolean_options = ['force', 'without-apidocs']

    def initialize_options(self):
        self.force = False
        self.without_apidocs = False
        self.outdir = None

    def finalize_options(self):
        if self.outdir is None:
            self.outdir = 'openravepy-html'

    def run(self):
        from docutils.core import publish_cmdline
        from docutils.nodes import raw
        from docutils.parsers import rst

        docutils_conf = os.path.abspath('docutils.ini')
        epydoc_conf = os.path.abspath('epydoc.config')
        epydoc_css = os.path.abspath('epydoc.css')
        epydoc_out = os.path.abspath(self.outdir)

        try:
            from pygments import highlight
            from pygments.lexers import get_lexer_by_name
            from pygments.formatters import HtmlFormatter

            def code_block(name, arguments, options, content, lineno,
                           content_offset, block_text, state, state_machine):
                lexer = get_lexer_by_name(arguments[0])
                html = highlight('\n'.join(content), lexer, HtmlFormatter())
                return [raw('', html, format='html')]
            code_block.arguments = (1, 0, 0)
            code_block.options = {'language' : rst.directives.unchanged}
            code_block.content = 1
            rst.directives.register_directive('code-block', code_block)
        except ImportError:
            print 'Pygments not installed, syntax highlighting disabled'

        for source in glob('*.txt'):
            dest = os.path.splitext(source)[0] + '.html'
            if self.force or not os.path.exists(dest) or \
                    os.path.getmtime(dest) < os.path.getmtime(source):
                print 'building documentation file %s' % dest
                publish_cmdline(writer_name='html',
                                argv=['--config=%s' % docutils_conf, source,
                                      dest])

        if not self.without_apidocs:
            try:
                from epydoc import cli
                old_argv = sys.argv[1:]
                sys.argv[1:] = [
                    '--config=%s' % epydoc_conf,
                    '--css=%s'%epydoc_css,
                    '--output=%s'%epydoc_out,
                    '--verbose'
                ]
                cli.cli()
                sys.argv[1:] = old_argv
            except ImportError:
                print 'epydoc not installed, skipping API documentation.'


class test_doc(Command):
    description = 'Tests the code examples in the documentation'
    user_options = []
    def initialize_options(self):
        pass
    def finalize_options(self):
        pass
    def run(self):
        for filename in glob('*.txt'):
            print 'testing documentation file %s' % filename
            doctest.testfile(filename, False, optionflags=doctest.ELLIPSIS)

setup(cmdclass={'build_doc': build_doc, 'test_doc': test_doc})

# requirements = []
# 
# setup(
#     name = 'OpenRAVE',
#     version = '0.2.2',
#     description = 'OpenRAVE Python bindings',
#     long_description = """Open Robotics Automation Virtual Environment""",
#     author = 'Rosen Diankov',
#     author_email = 'rosen.diankov@gmail.com',
#     license = 'LGPL + Apache License, Version 2.0',
#     url = 'http://openrave.programmingvision.com/',
#     zip_safe = True,
# 
#     classifiers = [
#     ],
#     packages = ['openravepy', 'openravepy.examples', 'openravepy.databases'],
#     test_suite = 'openravepy.tests',
# 
#     install_requires = requirements,
# 
#     entry_points = {
#         'console_scripts': None,
#     },
# 
#     cmdclass = {'build_doc': build_doc, 'test_doc': test_doc}
# )