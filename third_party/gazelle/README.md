# Gazelle Google APIs dependency patch

This patch is needed to add the googleapis dependency to the gazelle
Module file. Otherwise, gazelle dependencies do not see this
transitive dependency available, causing build errors.