Contributing
============

Chemfiles is an open-source project, that I make on my free time. Any
contribution, from documentation improvement to new features including bug fixes
are very welcome!

I would like to help, what can I do ?
-------------------------------------

Lot of things! Just pick one considering the time you can spend, and your
technical skills. And do not hesitate to come by out `gitter chat room`_ to say
hello and ask any question you can have!

.. _gitter chat room: https://gitter.im/chemfiles/chemfiles

Improving the code
^^^^^^^^^^^^^^^^^^

You can pick any `issue`_ in the list. `Help wanted`_ issues are specially
directed at first time contributors, and comes with step by step explanation of
how to solve the issue.

If you plan to add a new feature which is not in the issue list, please open a
new issue so that every one knows you are working on it, and so that the
implementation strategy can be discussed!

.. _issue: https://github.com/chemfiles/chemfiles/issues
.. _Help wanted: https://github.com/chemfiles/chemfiles/labels/Help%20wanted

Improve documentation
^^^^^^^^^^^^^^^^^^^^^

This documentation try to be easy to use, but there is always room for
improvements.  You can easily edit any :file:`.rst` file on `the github
repository <https://github.com/chemfiles/chemfiles/tree/master/doc>`_, and
propose your changes even with no git knowledge. All you need is a GitHub
account.

Share and spread the word
^^^^^^^^^^^^^^^^^^^^^^^^^

If you think that chemfiles is awesome, share it! The more users it will have,
the more features we can add to it, together!

External project used
---------------------

A few external projects are used in chemfiles development, and you will need a
bit of knowledge of them to contribute. Depending on what you want to do, not
all these projects are needed.

- Source code versioning: `git`_, together with `github`_ web interface for
  issues and pull requests.
- documentation: `sphinx`_ for generating HTML and PDF documentation, `Doxygen`_
  for documenting the source code, and `breathe`_ to use Doxygen content in
  sphinx.
- Build system: `cmake`_ is used as a cross-platform, cross-build system
  generator.
- Automatic testing: `Catch2`_ provide an nice unit test framework, and `travis`_
  run these tests each time the code is pushed to the repository.


.. _git: https://git-scm.com/
.. _github: https://github.com/about/
.. _sphinx: http://www.sphinx-doc.org/
.. _Doxygen: http://doxygen.org/
.. _breathe: https://breathe.readthedocs.io/
.. _cmake: https://cmake.org/
.. _Catch2: https://github.com/catchorg/Catch2
.. _travis: https://travis-ci.org/chemfiles/chemfiles/


Coding style, and other formating issues
----------------------------------------

When writing code for chemfiles, please respect the overall coding style. This
is not only a question of style, but make it easier to enter in the project if
all the files are formatted consistently. Do not use non standard features of
any programming language unless there is no other way to do it. In that case,
please wrap the code in ``#ifdef`` macros, and add cases for at least Linux,
macOS and Windows. You should add unit tests for each new piece of code, they
will be run on `travis`_ before your changes are merged.

Git messages should be informatives, and describe the *what*, not the *how*. If
your commit concern the documentation, please add the ``[doc]`` at the beginning
of the message.

Happy coding!
