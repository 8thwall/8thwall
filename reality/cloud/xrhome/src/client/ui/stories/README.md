# Overview

A replacement for our previous ui/playground based on Storybook.

We organize the smallest self-sufficient components under `UI/` while a page showing multiple
 components together are put under `Views/`.

The goal is.

* You can work on new and existing components under `UI/`.
* Incorporate them into kitchen-sink style page under `Views/` so you can see all their variations and how they act together. These pages contain components that are similar or of the
same group (e.g. form elements).
* Create custom pages under `Pages/` that can be used to display our design language. These pages
 will contain components across different groupings.

# Getting started

Run `npm run storybook` to serve and develop on these storybook components.
