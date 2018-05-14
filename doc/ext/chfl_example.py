from sphinx.directives.code import LiteralInclude

class Example(LiteralInclude):
    """
    TODO
    """

    def run(self):
        self.options["language"] = "cpp"
        self.options["start-after"] = "[example]"
        self.options["end-before"] = "[example]"
        self.options["dedent"] = 4
        return super(Example, self).run()


def setup(app):
    app.add_directive("chfl_example", Example)
