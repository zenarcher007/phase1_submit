#include <iostream>
#include <fstream>
#include <string>
#include <regex>

static std::string extract_text_field(const std::string& line) {
    
    static const std::regex full_text_re("\"full_text\"\\s*:\\s*\"((?:\\\\.|[^\"\\\\])*)\"");
    static const std::regex text_re("\"text\"\\s*:\\s*\"((?:\\\\.|[^\"\\\\])*)\"");

    std::smatch m;
    if (std::regex_search(line, m, full_text_re) && m.size() > 1) return m[1].str();
    if (std::regex_search(line, m, text_re) && m.size() > 1) return m[1].str();
    return line; // fallback for plain-text datasets
}

// Unescape minimal JSON escapes
static std::string unescape_json(std::string s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char c = s[i + 1];
            if (c == 'n') { out.push_back('\n'); i++; }
            else if (c == 't') { out.push_back('\t'); i++; }
            else if (c == 'r') { out.push_back('\r'); i++; }
            else if (c == '\\') { out.push_back('\\'); i++; }
            else if (c == '"') { out.push_back('"'); i++; }
            else { out.push_back(c); i++; } // simple fallback
        } else {
            out.push_back(s[i]);
        }
    }
    return out;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./extract <tweets.json|tweets.jsonl> <output_tokens.txt>\n";
        return 1;
    }

    const std::string in_path = argv[1];
    const std::string out_path = argv[2];

    std::ifstream fin(in_path);
    if (!fin) {
        std::cerr << "Error: cannot open input file: " << in_path << "\n";
        return 1;
    }

    std::ofstream fout(out_path);
    if (!fout) {
        std::cerr << "Error: cannot open output file: " << out_path << "\n";
        return 1;
    }

    // ECMAScript-safe patterns (no lookbehind)
    const std::regex hashtag_re(R"(#([A-Za-z0-9_]+))");
    const std::regex url_re(R"(https?://[^\s<>"\]\)]+)");

    std::string line;
    long long hashtag_count = 0, url_count = 0;

    while (std::getline(fin, line)) {
        std::string text = unescape_json(extract_text_field(line));
	
	for (std::sregex_iterator it(text.begin(), text.end(), hashtag_re), end; it != end; ++it) 
	{
    		fout << "#" << (*it)[1].str() << "\n";
		hashtag_count++;
	}
	for (std::sregex_iterator it(text.begin(), text.end(), url_re), end; it != end; ++it) 
	{
    		fout << (*it).str() << "\n";
		url_count++;
	}
      }
    

    std::cerr << "Extracted hashtags: " << hashtag_count << "\n";
    std::cerr << "Extracted URLs:     " << url_count << "\n";
    std::cerr << "Wrote tokens to:    " << out_path << "\n";
    return 0;
}
