from docutils import nodes, utils
from docutils.parsers.rst import Directive


class ChemfilesSelection(Directive):
    has_content = True
    required_arguments = 1
    final_argument_whitespace = True

    def run(self):
        self.assert_has_content()

        node = nodes.admonition('\n'.join(self.content))
        node.set_class("chemfiles-selection")

        title_text = self.arguments[0]
        textnodes, _ = self.state.inline_text(title_text, self.lineno)

        target = nodes.target('', '', ids=[nodes.make_id(title_text)])
        target.append(nodes.reference(
            '', '',
            refid=nodes.make_id(title_text),
            *textnodes,
        ))

        title = nodes.title(title_text, '', target)
        title['add_permalink'] = True
        node.append(title)

        self.state.nested_parse(self.content, self.content_offset, node)

        return [node]


def setup(app):
    app.add_directive('chemfiles-selection', ChemfilesSelection)
