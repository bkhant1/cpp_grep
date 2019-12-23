# cpp_grep

## In python
I ran accross a quite convenient function (called `grep`) in python. It recursively applies a predicate to a structure (list or dictionary) and returns element matching this predicate. 

```python
def grep(struct, predicate, results=[]):
  if predicate(struct):
    results.append(struct)
  if isinstance(struct, dict):
    for elem in struct.values():
      grep(elem, predicate, results)
  elif isinstance(struct, list):
    for elem in struct:
      grep(elem, predicate, results)
  return results

data = {
  "customers":[
    {
      "name": "Jon",
      "address": "j.doe@hotmail.eu"
    },
    {
      "name": "Denis",
      "address": "denis.dude@gmail.com"
    }
  ]
}

result = grep(
  data,
  lambda x: (
    isinstance(x, dict)
    and "name" in x 
    and x["name"] == "Denis")
)

# result == [{"name": "Denis", "address": "denis.dude@gmail.com"}]}
```

This comes quite handy for instance when testing APIs. In the example above we're expecting the `data` to have in one customer named Denis. Using grep the assertion is resilient to a change in the datastructure, and avoid having to "manually" look for it.

# In C++

This is an attempt to implement the same thing in C++. `grep` will iterate:
* through tuples (and through classes via reflection once implemented) at compile time
* through iterable containers at runtime

For each element encountered it will apply the predicate if the **type** of the element **can be passed** to the predicate. 

It puts a pointer to the element in the list it returns.

Pseudo-example inspired from the [tests](https://github.com/bkhant1/cpp_grep/blob/master/tests/grep.cpp) (the data declaration doesn't compile in C++11 but C++ syntax makes it a bit unclear in the test):

```cpp
tuple<
    vector<int>, char, tuple<
        int, string, vector<int>>>
    data {
    	{1,2,3,4,6},
    	'B',
    	{
    	    5,
    	    "Bonjour",
    	    {4,3,2,1}
	    }}
	    
auto result = grep<char>(data, equal_to<char>('B'))
// result contains the address of the 2 characters the address of the two 'B's in data. It took 1 + size("Bonjour") evaluation - one per character
```
