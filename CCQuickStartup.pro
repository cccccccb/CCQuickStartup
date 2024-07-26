TEMPLATE = subdirs

SUBDIRS += \
    plugin \
    src    \
    examples

plugin.depends = src
examples.depends = src
