<template>
  <div>
    <el-collapse v-model="activeNames" v-if="data">
      <el-collapse-item title="标题" name="title">
        {{ data.title }}
      </el-collapse-item>

      <el-collapse-item title="正文" name="content">
        <div v-html="data.content"></div>
      </el-collapse-item>
    </el-collapse>  
  </div>
</template>

<script>
import { mapActions } from 'vuex'
import { Loading } from 'element-ui'

export default {
  data () {
    return {
      data: null,
      activeNames: ['title', 'content']
    }
  },

  methods: {
    ...mapActions(['getDataItem'])
  },

  mounted () {
    let id = this.$route.params.id
    let loading = Loading.service()
    this.getDataItem(id).then(resp => {
      this.data = resp.data.data
    }).finally(() => loading.close())
  }
}
</script>
