#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/misc.h>
#include <string>

#include "authenticator.h"

std::string Authenticator::genSalt()
{
    CryptoPP::byte salt[8];
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(salt, sizeof(salt));

    std::string salt_hex;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(salt_hex));
    encoder.Put(salt, sizeof(salt));
    encoder.MessageEnd();

    return salt_hex;
}

std::string Authenticator::genHash(const std::string& salt, const std::string& password)
{
    std::string data = salt + password;

    CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256().CalculateDigest(digest, (const CryptoPP::byte*)data.data(), data.size());

    std::string hash_hex;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hash_hex));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return hash_hex;
}

bool Authenticator::checkHash(const std::string& salt, const std::string& password, const std::string& hash_client)
{
    std::string expected_hash = genHash(salt, password);

    if (expected_hash.size() != hash_client.size()) {
        return false;
    }

    return CryptoPP::VerifyBufsEqual((const CryptoPP::byte*)expected_hash.data(),
                (const CryptoPP::byte*)hash_client.data(), expected_hash.size());
}