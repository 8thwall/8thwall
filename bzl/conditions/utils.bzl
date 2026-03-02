def dict_union(list_of_dicts):
    z = {}
    for i in list_of_dicts:
        z.update(i)
    return z

# A function to merge multiple select statements
def merge_dicts_for_select(args):
    merged = {}
    for select_dict in args:
        for key, value in select_dict.items():
            if key in merged:
                merged[key] += value
            else:
                merged[key] = value

    return merged
