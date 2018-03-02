# Notes

This is a collection of thoughts and processes we had when making this part of
the application. It marks a good place to put weird artifacts that we found
about, say, libraries we used that were either too lengthy or innapropriate for
in-line comments.

## Week 6

The choice to go C++ with this portion was motivated by two reasons: 1. most of
the other code would also be in C++, since it lends itself nicely to image
processing and 2. fully fledged type safety and OOP. Looking back, maybe Java
would've been the better choice, since I am very familiar with it and it offers
all the safety I wanted, but the GTK3 libraries looked a little... shady.
Getting more into the weeds, I see that it would've been fine either way.
