""" Genrule that moves a file to a new location with a new name. """

def aliased_file(name, filename, source_target, source_filename = None, **kwargs):
    native.genrule(
        name = name,
        srcs = [
            source_target,
        ],
        outs = [
            filename,
        ],
        cmd = "cp $$(echo $(locations {target}) | cut -d' ' -f1 | xargs dirname)/{name} $@".format(
            target = source_target,
            name = source_filename if source_filename != None else filename,
        ),
        output_to_bindir = 1,
        **kwargs
    )
