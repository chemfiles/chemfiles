import sys

for line in sys.stdin:
    if 'warning: section' in line and 'is deprecated' in line:
        next(sys.stdin)
        next(sys.stdin)
    elif 'note: change section name to' in line:
        next(sys.stdin)
        next(sys.stdin)
    else:
        sys.stderr.write(line)
        sys.stderr.flush()
