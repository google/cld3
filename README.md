# Compact Language Detector v3 (CLD3)

* [Model](#model)
* [Installation](#installation)
* [Contact](#contact)
* [Credits](#credits)

### Model

CLD3 is a neural network model for language identification. This package
 contains the inference code and a trained model. The inference code
 extracts character ngrams from the input text and computes the fraction
 of times each of them appears. For example, as shown in the figure below,
 if the input text is "banana", then one of the extracted trigrams is "ana"
 and the corresponding fraction is 2/4. The ngrams are hashed down to an id
 within a small range, and each id is represented by a dense embedding vector
 estimated during training.

The model averages the embeddings corresponding to each ngram type according
 to the fractions, and the averaged embeddings are concatenated to produce
 the embedding layer. The remaining components of the network are a hidden
 (Rectified linear) layer and a softmax layer.

To get a language prediction for the input text, we simply perform a forward
 pass through the network.

![Figure](model.png "CLD3")

### Installation
CLD3 is designed to run in the Chrome browser, so it relies on code in
[Chromium](http://www.chromium.org/).
The steps for building and running the language detection model are:

- [check out](http://www.chromium.org/developers/how-tos/get-the-code) the
  Chromium repository.
- copy the code to `//third_party/cld_3`
- build and run the model using the commands:

```shell
gn gen out/Default
ninja -C out/Default third_party/cld_3/src:language_identifier_main
out/Default/language_identifier_main
```
### Contact

To ask questions or report issues please contact cld3-users@google.com.

### Credits

Original authors of the code in this package include (in alphabetical order):

* Alex Salcianu
* Andy Golding
* Anton Bakalov
* Chris Alberti
* Daniel Andor
* David Weiss
* Emily Pitler
* Greg Coppola
* Jason Riesa
* Kuzman Ganchev
* Michael Ringgaard
* Nan Hua
* Ryan McDonald
* Slav Petrov
* Stefan Istrate
* Terry Koo
