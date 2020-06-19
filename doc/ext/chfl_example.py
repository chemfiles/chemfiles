from sphinx.directives.code import LiteralInclude


class ChemfilesExample(LiteralInclude):
    """Add a code example for chemfiles documentation"""

    def run(self):
        self.options["language"] = "cpp"
        self.options["start-after"] = "[example]"
        self.options["end-before"] = "[example]"
        self.options["dedent"] = 4
        return super(ChemfilesExample, self).run()


def setup(app):
    app.add_directive("chemfiles-example", ChemfilesExample)
