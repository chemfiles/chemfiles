from docutils import nodes
from docutils.parsers.rst import Directive


class html_hidden(nodes.Element):
    pass


def visit_html_hidden_latex(self, node):
    self.body.append(node['before-not-html'])


def depart_html_hidden_latex(self, node):
    pass


def visit_html_hidden_html(self, node):
    self.body.append(
        '<details><summary><a>{}</a></summary>'.format(node['toggle'])
    )


def depart_html_hidden_html(self, node):
    self.body.append('</details>')


class HTMLHidden(Directive):
    node_class = html_hidden
    has_content = True
    required_arguments = 0
    optional_arguments = 0
    final_argument_whitespace = False
    option_spec = {
        'toggle': str,
        'before-not-html': str,
    }

    def run(self):
        node = html_hidden()
        node['toggle'] = self.options.get('toggle', 'click to show/hide')
        node['before-not-html'] = self.options.get('before-not-html', '')
        self.state.nested_parse(self.content, self.content_offset, node)
        return [node]


def setup(app):
    app.add_directive("htmlhidden", HTMLHidden)
    app.add_node(html_hidden,
                 html=(visit_html_hidden_html,
                       depart_html_hidden_html),
                 latex=(visit_html_hidden_latex,
                        depart_html_hidden_latex))
