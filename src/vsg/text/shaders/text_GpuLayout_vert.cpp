#include <vsg/io/VSG.h>
static auto text_GpuLayout_vert = []() {std::istringstream str(
R"(#vsga 0.0.2
Root id=1 vsg::ShaderStage
{
  NumUserObjects 0
  Stage 1
  EntryPoint "main"
  ShaderModule id=2 vsg::ShaderModule
  {
    NumUserObjects 0
    Source "#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

// specialization constants
layout(constant_id = 0) const uint numTextIndices = 1;

#define GLYPH_DIMENSIONS 0
#define GLYPH_BEARINGS 1
#define GLYPH_UVRECT 2

layout(set = 0, binding = 1) uniform sampler2D glyphMetricsSampler;

layout(set = 1, binding = 0) uniform TextLayout {
    vec4 position;
    vec4 horizontal;
    vec4 vertical;
    vec4 color;
    vec4 outlineColor;
    float outlineWidth;
} textLayout;

layout(set = 1, binding = 1) uniform TextIndices {
    uvec4 glyph_index[numTextIndices];
} text;


layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 outlineColor;
layout(location = 2) out float outlineWidth;
layout(location = 3) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    // compute the position of the glyph
    float horiAdvance = 0.0;
    float vertAdvance = 0.0;
    for(uint i=0; i<gl_InstanceIndex; ++i)
    {
        uint glyph_index = text.glyph_index[i / 4][i % 4];
        if (glyph_index==0)
        {
            // treat as a newlline
            vertAdvance -= 1.0;
            horiAdvance = 0.0;
        }
        else
        {
            horiAdvance += texture(glyphMetricsSampler, vec2(GLYPH_DIMENSIONS, glyph_index))[2];
        }
    }
    vec3 cursor = textLayout.position.xyz + textLayout.horizontal.xyz * horiAdvance + textLayout.vertical.xyz * vertAdvance;

    // compute the position of vertex
    uint glyph_index = text.glyph_index[gl_InstanceIndex / 4][gl_InstanceIndex % 4];

    vec4 dimensions = texture(glyphMetricsSampler, vec2(GLYPH_DIMENSIONS, glyph_index));
    vec4 bearings = texture(glyphMetricsSampler, vec2(GLYPH_BEARINGS, glyph_index));
    vec4 uv_rec = texture(glyphMetricsSampler, vec2(GLYPH_UVRECT, glyph_index));

    vec3 pos = cursor + textLayout.horizontal.xyz * (bearings.x + inPosition.x * dimensions.x) + textLayout.vertical.xyz * (bearings.y + (inPosition.y-1.0) * dimensions.y);

    gl_Position = (pc.projection * pc.modelview) * vec4(pos, 1.0);
    gl_Position.z -= inPosition.z*0.001;

    fragColor = textLayout.color;
    outlineColor = textLayout.outlineColor;
    outlineWidth = textLayout.outlineWidth;

    fragTexCoord = vec2(mix(uv_rec[0], uv_rec[2], inPosition.x), mix(uv_rec[1], uv_rec[3], inPosition.y));
}
"
    SPIRVSize 1402
    SPIRV 119734787 65536 524298 214 0 131089 1 393227 1 1280527431 1685353262 808793134
     0 196622 0 1 786447 0 4 1852399981 0 23 129 156
     184 188 191 197 196611 2 450 589828 1096764487 1935622738 1918988389 1600484449
     1684105331 1868526181 1667590754 29556 262149 4 1852399981 0 327685 8 1769107304 1635148865
     6644590 327685 10 1953654134 1635148865 6644590 196613 13 105 458757 23 1230990439
     1635021678 1231381358 2019910766 0 327685 28 1887005799 1852399464 7890276 393221 30 1416459630
     1232369765 1667851374 29541 327685 32 1954047316 1768189513 7562595 393222 32 0 1887005799
     1852399464 7890276 262149 34 1954047348 0 458757 55 1887005799 1952796008 1935894898 1886216531
     7497068 262149 72 1936880995 29295 327685 73 1954047316 1870225740 29813 393222 73
     0 1769172848 1852795252 0 393222 73 1 1769107304 1953394554 27745 393222 73
     2 1953654134 1818321769 0 327686 73 3 1869377379 114 458758 73 4
     1819571567 1130720873 1919904879 0 458758 73 5 1819571567 1466265193 1752458345 0 327685
     75 1954047348 1870225740 29813 327685 93 1887005799 1852399464 7890276 327685 102 1701669220
     1869181806 29550 327685 108 1918985570 1936158313 0 262149 114 1918858869 25445 196613
     121 7565168 327685 129 1867542121 1769236851 28271 393221 154 1348430951 1700164197 2019914866
     0 393222 154 0 1348430951 1953067887 7237481 196613 156 0 393221 158
     1752397136 1936617283 1953390964 115 393222 158 0 1785688688 1769235301 28271 393222 158
     1 1701080941 1701410412 119 196613 160 25456 327685 184 1734439526 1869377347 114
     393221 188 1819571567 1130720873 1919904879 0 393221 191 1819571567 1466265193 1752458345 0
     393221 197 1734439526 1131963732 1685221231 0 262215 23 11 43 262215 30
     1 0 262215 31 6 16 327752 32 0 35 0 196679
     32 2 262215 34 34 1 262215 34 33 1 262215 55
     34 0 262215 55 33 1 327752 73 0 35 0 327752
     73 1 35 16 327752 73 2 35 32 327752 73 3
     35 48 327752 73 4 35 64 327752 73 5 35 80
     196679 73 2 262215 75 34 1 262215 75 33 0 262215
     129 30 0 327752 154 0 11 0 196679 154 2 262216
     158 0 5 327752 158 0 35 0 327752 158 0 7
     16 262216 158 1 5 327752 158 1 35 64 327752 158
     1 7 16 196679 158 2 262215 184 30 0 262215 188
     30 1 262215 191 30 2 262215 197 30 3 131091 2
     196641 3 2 196630 6 32 262176 7 7 6 262187 6
     9 0 262165 11 32 0 262176 12 7 11 262187 11
     14 0 262165 21 32 1 262176 22 1 21 262203 22
     23 1 131092 26 262167 29 11 4 262194 11 30 1
     262172 31 29 30 196638 32 31 262176 33 2 32 262203
     33 34 2 262187 21 35 0 262187 11 37 4 262176
     41 2 11 262187 6 48 1065353216 589849 52 6 1 0
     0 0 1 0 196635 53 52 262176 54 0 53 262203
     54 55 0 262167 59 6 2 262167 61 6 4 262187
     11 63 2 262187 21 68 1 262167 70 6 3 262176
     71 7 70 524318 73 61 61 61 61 61 6 262176
     74 2 73 262203 74 75 2 262176 76 2 61 262187
     21 86 2 262187 21 95 4 262176 101 7 61 262187
     6 116 1073741824 262176 128 1 70 262203 128 129 1 262176
     130 1 6 262187 11 142 1 196638 154 61 262176 155
     3 154 262203 155 156 3 262168 157 61 4 262174 158
     157 157 262176 159 9 158 262203 159 160 9 262176 161
     9 157 262176 173 3 61 262187 6 177 981668463 262176 179
     3 6 262203 173 184 3 262187 21 185 3 262203 173
     188 3 262203 179 191 3 262187 21 192 5 262176 193
     2 6 262176 196 3 59 262203 196 197 3 262187 11
     207 3 327734 2 4 0 3 131320 5 262203 7 8
     7 262203 7 10 7 262203 12 13 7 262203 12 28
     7 262203 71 72 7 262203 12 93 7 262203 101 102
     7 262203 101 108 7 262203 101 114 7 262203 71 121
     7 196670 8 9 196670 10 9 196670 13 14 131321 15
     131320 15 262390 17 18 0 131321 19 131320 19 262205 11
     20 13 262205 21 24 23 262268 11 25 24 327856 26
     27 20 25 262394 27 16 17 131320 16 262205 11 36
     13 327814 11 38 36 37 262205 11 39 13 327817 11
     40 39 37 458817 41 42 34 35 38 40 262205 11
     43 42 196670 28 43 262205 11 44 28 327850 26 45
     44 14 196855 47 0 262394 45 46 51 131320 46 262205
     6 49 10 327811 6 50 49 48 196670 10 50 196670
     8 9 131321 47 131320 51 262205 53 56 55 262205 11
     57 28 262256 6 58 57 327760 59 60 9 58 458840
     61 62 56 60 2 9 327761 6 64 62 2 262205
     6 65 8 327809 6 66 65 64 196670 8 66 131321
     47 131320 47 131321 18 131320 18 262205 11 67 13 327808
     11 69 67 68 196670 13 69 131321 15 131320 17 327745
     76 77 75 35 262205 61 78 77 524367 70 79 78
     78 0 1 2 327745 76 80 75 68 262205 61 81
     80 524367 70 82 81 81 0 1 2 262205 6 83
     8 327822 70 84 82 83 327809 70 85 79 84 327745
     76 87 75 86 262205 61 88 87 524367 70 89 88
     88 0 1 2 262205 6 90 10 327822 70 91 89
     90 327809 70 92 85 91 196670 72 92 262205 21 94
     23 327815 21 96 94 95 262205 21 97 23 327819 21
     98 97 95 458817 41 99 34 35 96 98 262205 11
     100 99 196670 93 100 262205 53 103 55 262205 11 104
     93 262256 6 105 104 327760 59 106 9 105 458840 61
     107 103 106 2 9 196670 102 107 262205 53 109 55
     262205 11 110 93 262256 6 111 110 327760 59 112 48
     111 458840 61 113 109 112 2 9 196670 108 113 262205
     53 115 55 262205 11 117 93 262256 6 118 117 327760
     59 119 116 118 458840 61 120 115 119 2 9 196670
     114 120 262205 70 122 72 327745 76 123 75 68 262205
     61 124 123 524367 70 125 124 124 0 1 2 327745
     7 126 108 14 262205 6 127 126 327745 130 131 129
     14 262205 6 132 131 327745 7 133 102 14 262205 6
     134 133 327813 6 135 132 134 327809 6 136 127 135
     327822 70 137 125 136 327809 70 138 122 137 327745 76
     139 75 86 262205 61 140 139 524367 70 141 140 140
     0 1 2 327745 7 143 108 142 262205 6 144 143
     327745 130 145 129 142 262205 6 146 145 327811 6 147
     146 48 327745 7 148 102 142 262205 6 149 148 327813
     6 150 147 149 327809 6 151 144 150 327822 70 152
     141 151 327809 70 153 138 152 196670 121 153 327745 161
     162 160 35 262205 157 163 162 327745 161 164 160 68
     262205 157 165 164 327826 157 166 163 165 262205 70 167
     121 327761 6 168 167 0 327761 6 169 167 1 327761
     6 170 167 2 458832 61 171 168 169 170 48 327825
     61 172 166 171 327745 173 174 156 35 196670 174 172
     327745 130 175 129 63 262205 6 176 175 327813 6 178
     176 177 393281 179 180 156 35 63 262205 6 181 180
     327811 6 182 181 178 393281 179 183 156 35 63 196670
     183 182 327745 76 186 75 185 262205 61 187 186 196670
     184 187 327745 76 189 75 95 262205 61 190 189 196670
     188 190 327745 193 194 75 192 262205 6 195 194 196670
     191 195 327745 7 198 114 14 262205 6 199 198 327745
     7 200 114 63 262205 6 201 200 327745 130 202 129
     14 262205 6 203 202 524300 6 204 1 46 199 201
     203 327745 7 205 114 142 262205 6 206 205 327745 7
     208 114 207 262205 6 209 208 327745 130 210 129 142
     262205 6 211 210 524300 6 212 1 46 206 209 211
     327760 59 213 204 212 196670 197 213 65789 65592
  }
  NumSpecializationConstants 0
}
)");
vsg::VSG io;
return io.read_cast<vsg::ShaderStage>(str);
};
