# Contributing to chemfiles

:+1::tada: First off, thanks for taking the time to contribute to chemfiles! :tada::+1:

If you want to contribute but feel a bit lost, do not hesitate to
[contact][Gitter] us and ask your questions! We will happily mentor you through
your first contributions.

## Area of contributions

The first and best way to contribute to chemfiles is to use it and advertise it
to other potential users. Other than that, you can help with:

- the documentation: correcting typos, improving the sentences to make things
  more clear;
- bug fixes and improvement to existing C++ code;
- implementing new formats;
- improving the C interface;
- and many more …

All these contributions are very welcome. We accept contributions either via
Github pull request (have a look [here][PR] for Github model of pull request);
or you can send patches by email at luthaf@luthaf.fr.

If you want to work on the code and pick something easy to get started, have a
look at the [easy issues][easy-issues].


## Bug reports and feature requests

Bug and feature requests should be reported as [Github issue][issue]. For bugs,
you should provide information so that we can reproduce it: what did you try?
What did you expect? What happened instead? Please provide any useful code
snippet or input file with your bug report.

If you want to add a new feature to chemfiles, please create an [issue] so that
we can discuss it, and you have more chances to see your changes incorporated.

### Code contribution check-list

Every item in this list is explained in the next section

- [ ] Fork chemfiles;
- [ ] Create a local branch;
- [ ] Add code / correct typos / ...;
    - [ ] Add new tests with your code;
    - [ ] Add documentation for your code;
- [ ] Check that the tests still pass;
- [ ] Push to Github;
- [ ] Create a Pull-Request;
- [ ] Discuss your changes with the reviewers;
- [ ] Have your code merged in chemfiles
- [ ] Celebrate! :tada: :cake: :tada:

### Contribution tutorial

In this small tutorial, you should replace `<angle brackets>` as needed. If
anything is unclear, please [ask][Gitter] for clarifications! There are no dumb
questions.

---

Start by [forking chemfiles][fork], and then clone and build your fork locally
by running:

```bash
git clone https://github.com/<YOUR USERNAME>/chemfiles
cd chemfiles
mkdir build
cd build
cmake ..
make
```

Then create a new branch for your changes

```bash
git checkout -b <new-branch>
```

Do your changes, containing the documentation (in `doc`) and associated unit
tests (in `tests`) for new code.

Then, run all the test and check that they still pass:

```bash
cmake -DCHFL_BUILD_TESTS=ON .
make
ctest
```

Finally, you can push your code to Github, and create a [Pull-Request][PR] to
the `chemfiles/chemfiles` repository.

```bash
git commit  # ask for help if you don't know how to use git
git push -u origin <new-branch>
```

[Gitter]: https://gitter.im/chemfiles/chemfiles
[PR]: https://help.github.com/articles/using-pull-requests/
[easy-issues]: https://github.com/chemfiles/chemfiles/labels/Help%20wanted
[fork]: https://help.github.com/articles/fork-a-repo/
[issue]: https://github.com/chemfiles/chemfiles/issues/new
