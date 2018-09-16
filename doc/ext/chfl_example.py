from sphinx.directives.code import LiteralInclude


class CHFLExample(LiteralInclude):
    """Add a code example for chemfiles documentation"""

    def run(self):
        self.options["language"] = "cpp"
        self.options["start-after"] = "[example]"
        self.options["end-before"] = "[example]"
        self.options["dedent"] = 4
        return super(CHFLExample, self).run()


def setup(app):
    app.add_directive("chfl_example", CHFLExample)
