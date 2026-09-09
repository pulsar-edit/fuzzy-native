{
  'targets': [
    {
      'target_name': 'fuzzy-native',
      'include_dirs': [
        '<!@(node -p "require(\'node-addon-api\').include")'
      ],
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
      'cflags': [
        '-std=c++20',
        '-O3',
      ],
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS': [
          '-std=c++20',
          '-O3',
          '-stdlib=libc++',
        ],
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'sources': [
        'src/fuzzy-native.cpp',
        'src/fuzzaldrin_score.cpp',
        'src/score_match.cpp',
        'src/MatcherBase.cpp',
      ],
      'conditions': [
        ['OS == "win"', {
          'defines': ['_HAS_EXCEPTIONS'],
          'msvs_disabled_warnings': [
            4267,  # conversion from 'size_t' to 'int', possible loss of data
            4530,  # exception unwinding
          ],
        }],
        ['OS == "linux"', {
          'ldflags': [
            '-s',
          ],
        }],
      ],
    }
  ]
}
