#include "cyaml/cyaml.h"
#include "gtest/gtest.h"

struct Point
{
    float x;
    float y;
};

namespace cyaml
{
    template<>
    struct Converter<Point>
    {
        static Node encode(const Point &rhs)
        {
            Node ret;
            ret.push_back(rhs.x);
            ret.push_back(rhs.y);
            return ret;
        }

        static Point decode(const Node &rhs)
        {
            if (!rhs.is_seq() || rhs.size() != 2) {
                throw Convertion_Exception();
            }

            return {rhs[0].as<float>(), rhs[1].as<float>()};
        }
    };
} // namespace cyaml

TEST(custom_type_test, custom_type_test)
{
    auto node = cyaml::load("[1.0, 2.2]");
    Point point;
    ASSERT_NO_THROW(point = node.as<Point>(););
    EXPECT_FLOAT_EQ(point.x, 1.0);
    EXPECT_FLOAT_EQ(point.y, 2.2);

    node = Point{3, 4};
    ASSERT_NO_THROW(point = node.as<Point>(););
    EXPECT_FLOAT_EQ(point.x, 3);
    EXPECT_FLOAT_EQ(point.y, 4);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
