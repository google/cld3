"""Tests for gcld3."""

import gcld3
import unittest


class NnetLanguageIdentifierTest(unittest.TestCase):

  def testLangIdentification(self):
    detector = gcld3.NNetLanguageIdentifier(min_num_bytes=0, max_num_bytes=1000)
    sample = "This text is written in English."
    result = detector.FindLanguage(text=sample)
    self.assertEqual(result.language, "en")
    self.assertTrue(result.is_reliable)
    self.assertGreater(result.proportion, 0.99)
    self.assertGreater(result.probability, 0.90)

  def testEmptyString(self):
    detector = gcld3.NNetLanguageIdentifier(
        min_num_bytes=10, max_num_bytes=1000)
    sample = ""
    result = detector.FindLanguage(text=sample)
    self.assertEqual(result.language, "und")
    self.assertFalse(result.is_reliable)
    self.assertEqual(result.proportion, 0.0)
    self.assertEqual(result.probability, 0.00)

  def testLangsIdentification(self):
    detector = gcld3.NNetLanguageIdentifier(min_num_bytes=0, max_num_bytes=1000)
    sample = ("This piece of text is in English. Този текст е на " "Български.")
    results = detector.FindTopNMostFreqLangs(text=sample, num_langs=2)
    self.assertEqual(results[0].language, "bg")
    self.assertTrue(results[0].is_reliable)
    self.assertLess(results[0].proportion, 0.75)
    self.assertGreater(results[0].probability, 0.90)
    self.assertEqual(results[1].language, "en")
    self.assertTrue(results[1].is_reliable)
    self.assertLess(results[1].proportion, 0.75)
    self.assertGreater(results[1].probability, 0.90)


if __name__ == "__main__":
  unittest.main()
